//-------------------------------------------------------
// High Performance Computing (IS1240) - class project
// ALS with regularization - The Netflix recommendation system
// Maha ELBAYAD --- 2015
//-------------------------------------------------------
// Standard compiling:
//      mpic++ -std=c++11  ALSNetflix.cpp -o ALS -llapack -lblas
//      mpiexec -n <p> ./ALSN
// Debugging with gdb:
//      mpic++ -std=c++11 ALSNetflix.cpp -o ALS -llapack -lblas  -g
//      mpiexec -n <p> xterm -e gdb ./ALS
// Preprocessor directives : DOPRINT: print output and part of matrices to spot potential erros.
//                         : BUILDB : To build the binary files (then exit).
#include <mpi.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <map>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstring>
#include <numeric>
#include <cblas.h>
#include <vector>
#include <random>

using namespace std;
#define MASTER 0
#define TRAINING_PATH   "Netflix/training_set/"             //Path to the training set
#define TRAINING_FILE   "Netflix/training_set/%s"           //Training file name format
#define MAX_RATINGS     90436030                            //Total ratings in the training set
#define T_RATINGS     10044477                              //Total ratings in the test set
#define TOTAL_RATINGS   100480508                           //Total ratings (overall)
#define MAX_MOVIES      17771                               //Total movies (+1 for indexation)
#define MAX_USERS   480051                                  //Total users in the training set (+1 for indexation)
#define TOTAL_USERS 480190                                  //Total users (overall)
#define MAX_FEATURES 12                                     // The intermediate dimension.
#define MAX_ITERATIONS 20                                   // Upperbound on iterations number.
#define LAMBDA     .04                                      //The regularization parameter.
#define TOL     .0002                                        //The stopping criterion tolerance.
#define CHECK     .04                                       //Size of the follow-up subset to the training set

// For users indices - building binaries-------------------------------------------------------------------
typedef map<int, int> IdMap;
typedef IdMap::iterator IdItr;
bool comp(const pair<int,int> &p1, const pair<int,int> &p2 ){
    return p1.second<p2.second;
}

// Basic structures: Rating, User & Movie------------------------------------------------------------------
struct Node
{
    int         UserId;
    int       MovieId;
    int        Rating;
    Node(): UserId(0), MovieId(0), Rating(0){};
    Node(int U, int M, int R ): UserId(U), MovieId(M), Rating(R) {}
};

struct User {
    int         UserId;
    int         RatingCount=0;
    double         RatingAVG=0.0;
};

struct Movie {
    int         MovieId;
    int         RatingCount=0;
    double      RatingAVG=0.0;
};

//Major object for processors=========================================================================
class PJOB{
    public:
        int             rank;                           //rank
        int             p;                              //nb processors

        //Training set---------------------------------------------------------------------------------
        int             myRatingCountM=0;               //Ratings count (Movie/User) for partitioning
        int             myRatingCountU=0;

        Node*           myRatingsM;                     //Stored by movie
        Node*           myRatingsU;                     //Stored by user
        Movie           myMovies[MAX_USERS];            //Array of movies
        User            myUsers[MAX_USERS];             //Array of users
        int             MoviesIndex[MAX_MOVIES];        //Starting index for each movie in the binaries
        int             UsersCount[MAX_USERS];          //Intermediate array for users indices (ratings count)
        int             UsersIndex[MAX_USERS];          //Starting index for each user in the binaries
        int             myfirstUser;                    //First user for the processor
        int             myfirstMovie;                   //First movie for the processor
        int             numMovies;                      //Number of handled movies per processor
        int             numUsers;                       //Number of handled users per processor

        //Test set---------------------------------------------------------------------------------
        Node*           Test;                           //Ratings
        int             Tcount=0;                       //Ratings count
        Node*           myTest;                         //Ratings to process for each p
        int             myTestcount;
        int             myStartTest;
        double          SE_loc;
        double          SE_global;

        //Binaries preparation---------------------------------------------------------------------
        int             flatMapping[MAX_USERS];         //True users ids (sparse)
        IdMap           userIds;                        //Compact users indices
        vector<int>     shuffle_Users;                  //Shuffling for fair repartition among processors.


        //ALS Matrices--------------------------------------------------------------------------
        double          MoviesFeatures[MAX_MOVIES][MAX_FEATURES];               //M
        double          UsersFeatures[MAX_USERS][MAX_FEATURES];                 //U

        //RMSE follow-up-------------------------------------------------------------------------
        double          gloabal_s;                     //Sum of residual squares (global)
        double          local_s;                       //Sum of residual squares (local)
        int             check_size;                    //Size of the follow-up subset (global)
        int             check_local;                   //Size of the follow-up subset (local)
        vector<double>  RMSE;                          //RMSE history
        vector<int>     checkR;                        //Indices of the follow-up subset in the training set


        MPI_Datatype    mpi_Node, mpi_Movie, mpi_User; //Handlers for communication
        bool            DoPrint=false;                 //For preprocessor directive DOPRINT
        mt19937         rng;                           //The Mersenne Twister random engine for each processor

        //Constructor & destructor----------------------------------------------------------------------------
        PJOB(int r, int p);
        ~PJOB(void) { };

        //Functions-------------------------------------------------------------------------------
        void             LoadIndexBinaries();
        void             ProcessFile(char* movieFile, int movieId);
        void             LoadTrainingSet();
        void             BuildBfiles();
        void             BuildMPIData();
        void             LoadRatings();
        void             BuildMetaData();
        void             ALS();
        void             UpdateUsers();
        void             UpdateMovies();
        double           PredictRating(int movieId, int userId);
        void             FollowUp(int iter);
        void             ALSTest();

};

//Prototypes===========================================================================================
    void ProcessFile(char* movieFile, int movieId);
    void LoadTrainingSet();
    void BuildBfiles();
    bool Usercomp(Node i, Node j);
    void jobBroadcast(PJOB* job);
    //Auxiliaries:
    void pretty_print_matrix(double arr[][MAX_FEATURES], int firstRow, int lastRow, int firstColumn, int lastColumn);
    bool ParseInt(char* movieBuffer, int nLength, int &nPosition, int& nValue);
    bool ReadNumber(char* BufferIn, int nLength, int &nPosition, char* BufferOut);

// LAPACK Call===========================================================================================
extern "C" {
     static int dsysv(char UPLO, int N, int NRHS, double *A, int LDA, int *IPIV, double *B, int LDB,
            double *WORK, int LWORK) {

        extern void dsysv_(const char *UPLOp, const int *Np, const int *NRHSp, double *A, const int *LDAp,
                int *IPIV, double *B, const int *LDBp, double *WORK, const int *LWORKp, int *INFOp);
        int info;
        dsysv_(&UPLO, &N, &NRHS, A, &LDA, IPIV, B, &LDB, WORK, &LWORK, &info);
        return info;
    }
}

//===================================//
//               Main                //
//===================================//

    int main(int argc, char* argv[]){
        bool BuildBinaries=false;
        int p,rank;
        ofstream report;
        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &p);
        double start, end;
        PJOB* job = new PJOB(rank, p);
        #ifdef DOPRINT
            job->DoPrint=true;
        #endif
        //======== Phase 1 - Building the binaries
        #ifdef BUILDB
            BuildBinaries=true;
        #endif

        if(BuildBinaries && rank==MASTER){
            job->myRatingsU=new Node[TOTAL_RATINGS];
            job->myRatingsM=new Node[TOTAL_RATINGS];
            job->Test=new Node[TOTAL_RATINGS];
            job->LoadTrainingSet();
            job->BuildBfiles();
            }
        //======== Phase 1' - The ALS
        if(!BuildBinaries){
        if(rank==MASTER)
            job->LoadIndexBinaries();
        MPI_Barrier(MPI_COMM_WORLD);
        jobBroadcast(job);
        MPI_Barrier(MPI_COMM_WORLD);

        //Stats: useful for tuning lambda and tolerance and keep track of RMSE history
        if(rank==MASTER) {
            report.open("report.txt");
            report<<"#Processors="<<job->p<<endl;
            report<<"#Features="<<MAX_FEATURES<<endl;
            report<<"LAMBDA="<<LAMBDA<<endl;
            report<<"Tolerance="<<TOL<<endl;
            start=MPI_Wtime();
        }
        job->LoadRatings();
        if (rank==MASTER){
            end=MPI_Wtime();
            if(job->DoPrint) report<<"Loading ratings time: "<<end-start<<endl;
            start=MPI_Wtime();
        }
        job->ALS();
        if(rank==MASTER){
            end=MPI_Wtime();
            report<<"ALS time: "<<end-start<<endl;
            for(int i=1; i<job->RMSE.size();i++)
                if(job->RMSE[i]) report<<"RMSE["<<i<<"]="<<job->RMSE[i]<<endl;
        }
        delete[] job->myRatingsU; delete[] job->myRatingsM;

        //======== Phase 2 - Prediction on the test set
        if(rank==MASTER) start=MPI_Wtime();
        job->ALSTest();
        if(rank==MASTER){
            end=MPI_Wtime();
            report<<"Prediction time: "<<end-start<<endl;
            double RMSE=sqrt(job->SE_global/T_RATINGS*50);
            printf("[p%d]\tTest prediction RMSE= %.4f\n",rank,RMSE);
            report<<"Test set RMSE: "<<RMSE<<endl;
        }

        }
        MPI_Finalize();
        return EXIT_SUCCESS;
    }

//====================================================================//
//                          PJOB Functions                            //
//===================================================================//
//=============== Constructor

    PJOB::PJOB(int r, int numP) {
        rank = r;
        p = numP;
        rng.seed((r+2)*(r+3));
        BuildMPIData();
    }
//=========== Binary files preparation

    void PJOB::ProcessFile(char* movieFile, int movieId){
        FILE *stream;
        IdItr itr;
        char movieBuffer[1000];
        sprintf(movieBuffer,TRAINING_FILE,movieFile);
        int userId, rating, pos, uid, moviecount = 0;
        double tmp;
        uniform_real_distribution<double> unif(0, 1);
        printf("Procssing movie: %d\n", movieId);

        if (!(stream = fopen(movieBuffer, "r"))) {
            cout << "Failed to open " << movieBuffer << endl;
            getchar();
            return;
        }

        // Get movie id from first line:
        fgets(movieBuffer, 1000, stream);
        fgets(movieBuffer, 1000, stream);
        while ( !feof( stream ) )
        {
            pos = 0;
            ParseInt(movieBuffer, (int)strlen(movieBuffer), pos, userId);
            ParseInt(movieBuffer, (int)strlen(movieBuffer), pos, rating);

            //Pick whether it's in the training set or not:
            tmp=unif(rng);
            if (tmp <.9){                                                   // To the training set
            myRatingsU[myRatingCountM].MovieId = (int)movieId;
            myRatingsM[myRatingCountM].MovieId = (int)movieId;

            itr =userIds.find(userId);
            if (itr == userIds.end()) {
                //pick a random unchosen id from authorized range:
                uid = shuffle_Users[(int)userIds.size()];
                userIds[userId]=uid;
                UsersCount[uid]=1;
            }
            else {
                uid= itr->second;
                UsersCount[uid]++;
            }

            myRatingsU[myRatingCountM].UserId = uid;
            myRatingsM[myRatingCountM].UserId = uid;

            myRatingsU[myRatingCountM].Rating = (int)rating;
            myRatingsM[myRatingCountM].Rating = (int)rating;

            myRatingCountM++;

            moviecount++;
        }
        else{                                                               //To the test set
            Test[Tcount].MovieId=(int)movieId;
            Test[Tcount].UserId=(int)userId;
            Test[Tcount].Rating=(int)rating;
            Tcount++;
        }
            fgets(movieBuffer, 1000, stream);

        }
        MoviesIndex[movieId]=myRatingCountM-moviecount;

        fclose(stream);
    }

    void PJOB::LoadTrainingSet(){
        //First and only shuffling:
        shuffle_Users.resize(MAX_USERS);
        iota(shuffle_Users.begin(), shuffle_Users.end(), 1);
        shuffle(shuffle_Users.begin(), shuffle_Users.end(), mt19937{random_device{}()});
        char filename[15];
        for (int movieId = 1; movieId < MAX_MOVIES; movieId++) {
            sprintf(filename, "mv_%07d.txt", movieId);
            ProcessFile(filename,movieId);
        }
        if(DoPrint){
        printf("myRatingCountM=%d and Tcount=%d\n",myRatingCountM,Tcount);
        printf("Total Users =%d\n",(int)userIds.size());
        }
    }
//=========== Binary files writing
    void PJOB::BuildBfiles(){
        ofstream mapping,verifyC;
        FILE *ratingsFile,   // R sorted by movie
             *ratingsFile2,  // R sorted by user
             *moviesFile,    // Movies indices
             *usersFile, // Users indices
             *usersMapping, //Users pair indices for prediction phase.
             *test;     // Test set

        //Ratings per movie
        ratingsFile = fopen("ratings.bin","w");
        for (int i=0; i<myRatingCountM; i++) {
            Node* rating = myRatingsM + i;
            fwrite(rating, 3*sizeof(int), 1, ratingsFile);
        }
        fclose(ratingsFile);

        // movies indices
        moviesFile = fopen("movies.bin","w");
        fwrite(MoviesIndex, sizeof(int), sizeof(MoviesIndex), moviesFile);
        fclose(moviesFile);

        //Rating per user
        sort(&myRatingsU[0], &myRatingsU[myRatingCountM], Usercomp);
        ratingsFile2 = fopen("ratings2.bin","w");
        for (int i=0; i<myRatingCountM; i++) {
            Node* rating = myRatingsU + i;
            fwrite(rating, 3*sizeof(int), 1, ratingsFile2);
        }
        fclose(ratingsFile2);

        //Users indices
        if(DoPrint) verifyC.open("verifyC.txt");
        UsersCount[0]=0;
        partial_sum (UsersCount, UsersCount+(int)userIds.size(), UsersIndex);
        for(int i=MAX_USERS-1; i>1;i--){
            UsersIndex[i]=UsersIndex[i-1];
        }
        UsersIndex[1]=0;
        if(DoPrint){
        	for(int i=1; i<MAX_USERS;i++)
            	verifyC<<i<<" count:"<<UsersCount[i]<<" start index:"<<UsersIndex[i]<<endl;
            }
        usersFile = fopen("users.bin","w");
        fwrite(UsersIndex, sizeof(int), sizeof(UsersIndex), usersFile);
        fclose(usersFile);

        //users mapping
        if(DoPrint) mapping.open("mapping.txt");
        usersMapping = fopen("usersMap.bin","w");
        vector<pair<int, int>> v;
        copy(userIds.begin(),userIds.end(),back_inserter(v));
        sort(v.begin(),v.end(),comp);
        for(int i=0;i<v.size();i++) {
            fwrite(&(v[i].first), sizeof(int), 1, usersMapping );
            if(DoPrint){
                mapping<<i<<"->"<<v[i].first<<endl;
                }
        }
        if(DoPrint) fclose(usersMapping);
        //Test set:
        test = fopen("test.bin","w");
        for (int i=0; i<Tcount; i++) {
            Node* rating = Test + i;
            fwrite(rating, 3*sizeof(int), 1, test);
        }
        fclose(test);
    }
//=========  Binary files reading

    void PJOB::LoadIndexBinaries(){
        FILE *iFile;
        size_t result;
        //Movies Indices
        cout << "Loading Movies Indices" << endl;
        iFile = fopen("movies.bin","rb");
        if (iFile==NULL) { cout << "File error, movies.bin" << endl; exit(1);}
            result = fread(MoviesIndex, sizeof(int), MAX_MOVIES, iFile);
        if (result != MAX_MOVIES) { cout << "Reading error, movies.bin " << result << endl; exit(3);}
        fclose(iFile);

        //Users indices
        cout << "Loading Users Indices" << endl;
        iFile = fopen("users.bin","rb");
        if (iFile==NULL) { cout << "File error, users.bin" << endl; exit(1);}
            result = fread(UsersIndex, sizeof(int), MAX_USERS, iFile);
        if (result != MAX_USERS) { cout << "Reading error, users.bin " << result << endl; exit(3);}
        fclose(iFile);

        //Users mapping
        cout << "Loading Users Mapping" << endl;
        iFile = fopen("usersMap.bin","rb");
        if (iFile==NULL) { cout << "File error, usersMap.bin" << endl; exit(1);}
            result = fread(flatMapping, sizeof(int),MAX_USERS, iFile);
        fclose(iFile);
    }
//================ MPI DATA TYPES:
    void PJOB::BuildMPIData() {
        //Syntax--------------------------------------------
            //MPI_Type_struct(int count,
                       // const int *array_of_blocklengths,
                       // const MPI_Aint *array_of_displacements,
                       // const MPI_Datatype *array_of_types,
                       // MPI_Datatype *newtype)
        int blockLengths[3] = {1, 1, 1}; //mpi_Node Movie and User

        MPI_Aint disps[3] = {0, sizeof(int), 2*sizeof(int)};

        MPI_Datatype originalN[3] = {MPI_INT, MPI_INT, MPI_INT};
        MPI_Datatype original[3] = {MPI_INT, MPI_INT, MPI_DOUBLE};
        MPI_Type_create_struct(3, blockLengths, disps, originalN, &mpi_Node);
        MPI_Type_create_struct(3, blockLengths, disps, original, &mpi_Movie);
        MPI_Type_create_struct(3, blockLengths, disps, original, &mpi_User);
        MPI_Type_commit(&mpi_Node); MPI_Type_commit(&mpi_Movie); MPI_Type_commit(&mpi_User);
    }
//================ Load Ratings
    //Not Working!!
       /* void PJOB::LoadRatings() {
            MPI_File ratingsFile;
            MPI_Status *st;
            string str1="ratings.bin";
            string str2="ratings2.bin";
            char* filename1=const_cast<char*>(str1.c_str());
            char* filename2=const_cast<char*>(str2.c_str());

            try{ myRatingsU=new Node[myRatingCountU];}
            catch (bad_alloc&) {printf("Memory Allocation Error\n"); exit(1);}
            try{ myRatingsM=new Node[myRatingCountM];}
            catch (bad_alloc&) {printf("Memory Allocation Error\n"); exit(1);}
            MPI_Offset startRatingM = (MoviesIndex[myfirstMovie])*sizeof(Node),
                       startRatingU = (UsersIndex[myfirstUser])*sizeof(Node);
            printf("[%d]\t I start at %lld - for %d Nodes\n",rank,startRatingM,myRatingCountM);
            MPI_File_open(MPI_COMM_WORLD,filename1,MPI_MODE_RDONLY,MPI_INFO_NULL,&ratingsFile);
            if (ratingsFile==NULL) {
                printf("File error, ratings.bin\n");
                exit(1);
            }
            printf("[%d]\t \"ratings.bin\" open \n",rank);

            //Syntax:------------------------------------------------------------------------
                //int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf,
                    //int count, MPI_Datatype datatype,
                    //MPI_Status *status)
                //------------------------------------------------------------------------
            int err=MPI_File_read_at_all(ratingsFile,startRatingM,myRatingsM,myRatingCountM,mpi_Node,st);
            if(err!= MPI_SUCCESS)
                printf("Error reading ratings.bin\n");
            MPI_File_close(&ratingsFile);
            MPI_File_open(MPI_COMM_WORLD,filename2,MPI_MODE_RDONLY,MPI_INFO_NULL,&ratingsFile);
            if (ratingsFile==NULL) {
                printf("File error, ratings2.bin\n");
                exit(1);
            }
            printf("[%d]\t \"ratings2.bin\" open \n",rank);
            err=MPI_File_read_at_all(ratingsFile,startRatingU,myRatingsU,myRatingCountU,mpi_Node,st);
            if(err!= MPI_SUCCESS)
                printf("Error reading ratings2.bin\n");
            MPI_File_close(&ratingsFile);
        }*/

    void PJOB::LoadRatings(){
        try{ myRatingsU=new Node[myRatingCountU];}
        catch (bad_alloc&) {printf("Memory Allocation Error\n"); exit(1);}
        try{ myRatingsM=new Node[myRatingCountM];}
        catch (bad_alloc&) {printf("Memory Allocation Error\n"); exit(1);}
        int startRatingM = (MoviesIndex[myfirstMovie])*sizeof(Node),
            startRatingU = (UsersIndex[myfirstUser])*sizeof(Node);

        FILE *ratingsFile;
        size_t result;
        //Ratings by movie
        ratingsFile = fopen("ratings.bin","rb");
        printf("[p%d]\tLoading ratings by movie\n",rank);
        if (ratingsFile==NULL) {
            printf("File error, ratings.bin\n");
            exit(1);
        }
        fseek(ratingsFile, startRatingM, SEEK_SET);
        result = fread(myRatingsM, sizeof(Node), myRatingCountM, ratingsFile);
        if ((int) result != myRatingCountM){
            printf("Reading error - ratings.bin\n");
            exit(3);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        fclose(ratingsFile);

        //Ratings by user
        ratingsFile = fopen("ratings2.bin","rb");
         printf("[p%d]\tLoading ratings by user\n",rank);
        if (ratingsFile==NULL) {
            printf("File error, ratings2.bin\n");
            exit(1);
        }
        fseek(ratingsFile, startRatingU, SEEK_SET);
        result = fread(myRatingsU, sizeof(Node), myRatingCountU, ratingsFile);
        if ((int) result != myRatingCountU){
            printf("Reading error - ratings2.bin\n");
            exit(3);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        fclose(ratingsFile);
        }
//================ Build Movies/Users metaData and initialize M
    void PJOB::BuildMetaData(){
        normal_distribution<double> normal_dist(0, 1);
        string filename;
        ofstream meta;
        //For generating some movie stats
        /*if(DoPrint){
            filename="Movies_meta_"+to_string(rank)+".txt";
            meta.open(filename);
        }*/
        int count=0, mv;
        //Movies
        while(count<myRatingCountM){
            mv=myRatingsM[count].MovieId;
                while(myRatingsM[count].MovieId==mv){
                    myMovies[mv].RatingAVG+=myRatingsM[count].Rating;
                    myMovies[mv].RatingCount++;
                    count++;
                }
                myMovies[mv].RatingAVG=(double)myMovies[mv].RatingAVG/myMovies[mv].RatingCount;
                myMovies[mv].MovieId=mv;
                /*if(DoPrint)
                    meta<<"Movie "<<mv<<": "<<myMovies[mv].RatingCount<<" "<<myMovies[mv].RatingAVG<<endl;*/

                //Init M=============================
                MoviesFeatures[mv][0]=myMovies[mv].RatingAVG;
                for (int f=1; f<MAX_FEATURES; f++)
                    MoviesFeatures[mv][f] = normal_dist(rng);
        }
        if(DoPrint){
            printf("[p%d]\tMovies matrix initialized\n",rank);
            for(int i=myfirstMovie; i<myfirstMovie+10;i++){
                printf("[%d]\t",i);
                for(int j=0; j<5;j++)
                    printf("%.2f ",MoviesFeatures[i][j]);
                printf("\n");
            }
            meta.close();
        }
        //Users
        //For generating some users stats
        /*if(DoPrint){
            filename="Users_meta_"+to_string(rank)+".txt";
            meta.open(filename);
        }*/
        count=0; int us;
        while(count<myRatingCountU){
            us=myRatingsU[count].UserId;
            while(myRatingsU[count].UserId==us){
                myUsers[us].RatingAVG+=myRatingsU[count].Rating;
                myUsers[us].RatingCount++;
                count++;
            }
            myUsers[us].RatingAVG=(double) myUsers[us].RatingAVG/myUsers[us].RatingCount;
            myUsers[us].UserId=us;
            /*if(DoPrint)
                meta<<"User "<<us<<": "<<myUsers[us].RatingCount<<" "<<myUsers[us].RatingAVG<<endl;*/

            }
        printf("[p%d]\tDone building metadata\n",rank);
        if(DoPrint) meta.close();
    }
//================ ALS
    void PJOB::ALS(){
        //Syntax---------------------------------------------------------------------
            //int MPI_Allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                       //void *recvbuf, const int *recvcounts, const int *displs,
                       //MPI_Datatype recvtype, MPI_Comm comm)
            //---------------------------------------------------------------------
        //Counts and offsets for gathering:
        int nM=(int) (MAX_MOVIES-1)/p,nU=(int) (MAX_USERS-1)/p;
        int* recvcountUsers, * recvcountMovies, * displsMovies, * displsUsers;
        recvcountUsers=new int[p];
        recvcountMovies=new int[p];
        displsMovies=new int[p];
        displsUsers=new int[p];
        RMSE.resize(MAX_ITERATIONS+1);
        RMSE[0]=0.0;
        for (int i=0; i<p; i++) {
            recvcountMovies[i]=nM*MAX_FEATURES;
            recvcountUsers[i]=nU*MAX_FEATURES;
            displsMovies[i]=i*nM*MAX_FEATURES;
            displsUsers[i]=i*nU*MAX_FEATURES;
        }

        recvcountMovies[p-1]+=((MAX_MOVIES-1)%p)*MAX_FEATURES;
        recvcountUsers[p-1]+=((MAX_USERS-1)%p)*MAX_FEATURES;

        BuildMetaData();
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgatherv(&MoviesFeatures[myfirstMovie][0], recvcountMovies[rank], MPI_DOUBLE, &MoviesFeatures[1][0], recvcountMovies, displsMovies, MPI_DOUBLE, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
        if(rank==MASTER) printf("[p%d]\tMovies matrix gathered\n",rank);
        if(DoPrint) {
            printf("[p%d]\tMovies matrix - first 10x5\n",rank);
            pretty_print_matrix(MoviesFeatures,1,10,0,5);}
        MPI_Barrier(MPI_COMM_WORLD);
        //============================================//
        //                 Main Loop                  //
        //============================================//
        for (int iter=1; iter<=MAX_ITERATIONS; iter++) {
            if(rank==MASTER) printf("[p%d]\tIteration %d\n",rank,iter);
            UpdateUsers();
            MPI_Barrier(MPI_COMM_WORLD);
            if(rank==MASTER) printf("[p%d]\tUsers updated (%d)\n",rank,iter);
            MPI_Allgatherv(&UsersFeatures[myfirstUser][0], recvcountUsers[rank], MPI_DOUBLE, &UsersFeatures[1][0], recvcountUsers, displsUsers, MPI_DOUBLE, MPI_COMM_WORLD);
            if(DoPrint){
                printf("[p%d]\tUsers matrix - first 10x5 (%d)\n",rank,iter);
                pretty_print_matrix(UsersFeatures,1,10,0,5);
            }
            MPI_Barrier(MPI_COMM_WORLD);
            UpdateMovies();
            MPI_Barrier(MPI_COMM_WORLD);
            if (rank==MASTER) printf("[p%d]\tMovies updated (%d)\n",rank,iter);
            MPI_Allgatherv(&MoviesFeatures[myfirstMovie][0], recvcountMovies[rank], MPI_DOUBLE, &MoviesFeatures[1][0], recvcountMovies, displsMovies, MPI_DOUBLE, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);
            if(DoPrint){
                printf("[p%d]\tMovies matrix - first 10x5 (%d)\n",rank,iter);
                pretty_print_matrix(MoviesFeatures,1,10,0,5);
            }
            FollowUp(iter);
            if (rank==MASTER) {RMSE[iter]=sqrt(gloabal_s/check_size); printf("[p%d]\tRMSE(%d)=%.3f\n",rank,iter,RMSE[iter]);}
            MPI_Bcast (&RMSE[iter],1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            if(abs(RMSE[iter-1]-RMSE[iter])<TOL) break; //The RMSE has converged.
            MPI_Barrier(MPI_COMM_WORLD);
            //Keepp the last version of U and M
            MPI_Allgatherv(&MoviesFeatures[myfirstMovie][0], recvcountMovies[rank], MPI_DOUBLE, &MoviesFeatures[1][0], recvcountMovies, displsMovies, MPI_DOUBLE, MPI_COMM_WORLD);
            MPI_Allgatherv(&UsersFeatures[myfirstUser][0], recvcountUsers[rank], MPI_DOUBLE, &UsersFeatures[1][0], recvcountUsers, displsUsers, MPI_DOUBLE, MPI_COMM_WORLD);
        }
        delete [] recvcountUsers; delete [] displsUsers; delete [] recvcountMovies; delete [] displsMovies;
    }
//================ ALS - Update U
    void PJOB::UpdateUsers(){
        const int nf=MAX_FEATURES;
        double A[nf*nf],u[nf], v[nf];
        double * Mi=NULL;
        double * R=NULL;
        int r=0,info,us,ct;
        int ipiv[nf];
        int lwork=10000;
        double work[lwork];
        //prepare matrices and vectors:
        while (r < myRatingCountU) {
            do{
                us = myRatingsU[r].UserId;
                ct = (us <= 0 || us >= MAX_USERS) ? MAX_MOVIES : myUsers[us].RatingCount;
                r++;
            }
            while((ct <= 0 || ct >= MAX_MOVIES) && r < myRatingCountU);
            if (ct <= 0 || ct >= MAX_MOVIES || r >= myRatingCountU) break;
            else r--;
            //Allocate M_i- Build A:
            try{ Mi=new double[nf*ct];}
            catch (bad_alloc&) {printf("Memory allocation error\n"); exit(1);}
            try{R=new double[ct];}
            catch (bad_alloc&) {printf("Memory allocation error\n"); exit(1);}
            for(int i=0;i<nf;i++){
                u[i]=v[i]=0;
                for(int j=0;j<nf;j++){
                    if(i==j) A[i*nf+j]=1;
                    else A[i*nf+j]= 0;
                    }
                }
            for (int i=0; us == myRatingsU[r].UserId && r < myRatingCountU; i++) {
	            for (int f=0; f<nf; f++)
	                Mi[f*ct + i] = MoviesFeatures[myRatingsU[r].MovieId][f];
	            R[i] = double(myRatingsU[r].Rating);
	            r++;
            }

            //Syntax---------------------------------------------------------------------
                //cblas_dsyrk : multiplies a symmetric matrix by its transpose and adds a second matrix
                //==============================
                // void cblas_ssyrk ( const enum CBLAS_ORDER __ Order , const enum CBLAS_UPLO __ Uplo , const enum CBLAS_TRANSPOSE __ Trans , const int __ N , const int __ K , const double __ alpha , const double *__ A , const int __ lda , const double __ beta , double *__ C , const int __ ldc );
                //---------------------------------------------------------------------
            //A=Mi*Mi^T+lambda*ct*A.
            cblas_dsyrk(CblasRowMajor, CblasUpper, CblasNoTrans, nf, ct, 1.0, Mi, ct, LAMBDA*ct, A, nf);
            //Syntax---------------------------------------------------------------------
                //cblas_dgemv : Multiplies a matrix by a vector (single precision).
                //==============================
                // void cblas_sgemv ( const enum CBLAS_ORDER __ Order , const enum CBLAS_TRANSPOSE __ TransA , const int __ M , const int __ N , const double __ alpha , const double *__ A , const int __ lda , const double *__ X , const int __ incX , const double __ beta , double *__ Y , const int __ incY );
                //---------------------------------------------------------------------
            //v=Mi*R^T
            cblas_dgemv(CblasRowMajor, CblasNoTrans, nf, ct, 1.0, Mi, ct, R, 1, 0.0, v, 1);
            //Syntax---------------------------------------------------------------------
                //DSYSV - compute the solution to a real system of linear equations  A * X = B,
                //dsysv(UPLO,N,NRHS,A,LDA,IPIV,B,LDB,WORK,LWORK,INFO)
                // UPLO:'U' or 'L' for factorization method.
                // N number of linear equation in A - NRHS (right hand side |columns|)
                // LDA leading dimension of A and LDB of B
                // IPIV integer array of size N [out] Work double array [out]
                //LWORK length of work  LWORK >= max(1,N*NB) NB is the optimal blocksize for DSYTRF.
                //info int [out]
                //---------------------------------------------------------------------
            info = dsysv('L', nf, 1, A, nf, ipiv, v, nf, work, lwork);
            for (int i=0; i<nf; i++){
                UsersFeatures[us][i]=v[i];
            }
        delete[] Mi; Mi = NULL;
        delete[] R, work;
        }
    }
//================ ALS - Update M
    void PJOB::UpdateMovies(){
        const int nf=MAX_FEATURES;
        double A[nf*nf],u[nf], v[nf];
        double * Ui=NULL;
        double * R=NULL;
        int r=0,info,mv,ct;
        int ipiv[nf];
        int lwork=10000;
        double work[lwork];
        //prepare matrices and vectors:
        while (r < myRatingCountM) {
            do{
                mv = myRatingsM[r].MovieId;
                ct = (mv <= 0 || mv >= MAX_MOVIES) ? MAX_USERS : myMovies[mv].RatingCount;
                r++;
            }
            while((ct <= 0 || ct >= MAX_USERS) && r < myRatingCountM);
            if (ct <= 0 || ct >= MAX_USERS || r >= myRatingCountM) break;
            else r--;
            //Allocate M_i- Build A:
            try{ Ui=new double[nf*ct];}
            catch (bad_alloc&) {printf("Memory allocation error\n"); exit(1);}
            try{R=new double[ct];}
            catch (bad_alloc&) {printf("Memory allocation error\n"); exit(1);}
            for(int i=0;i<nf;i++){
                u[i]=v[i]=0;
                for(int j=0;j<nf;j++){
                    if(i==j) A[i*nf+j]=1;
                    else A[i*nf+j]= 0;
                    }
                }
            for (int i=0; mv == myRatingsM[r].MovieId && r < myRatingCountM; i++) {
            for (int f=0; f<nf; f++)
                Ui[f*ct + i] = UsersFeatures[myRatingsM[r].UserId][f];
                R[i] = double(myRatingsM[r].Rating);
                r++;
            }
            cblas_dsyrk(CblasRowMajor, CblasUpper, CblasNoTrans, nf, ct, 1.0, Ui, ct, LAMBDA*ct, A, nf);
            cblas_dgemv(CblasRowMajor, CblasNoTrans, nf, ct, 1.0, Ui, ct, R, 1, 0.0, v, 1);
            info = dsysv('L', nf, 1, A, nf, ipiv, v, nf, work, lwork);
            for (int i=0; i<nf; i++){
                MoviesFeatures[mv][i]=v[i];
            }

            delete[] Ui; Ui = NULL;
            delete[] R, work;
        }
    }
//================ Predict:
    double PJOB::PredictRating(int movieId, int userId){
        double s=0;
        int x;
        for (int i = 0; i < MAX_USERS-1; i++)
            if (userId == flatMapping[i]){
                x=i;
                break;
            }
        //Handling new users or new movies (rare)
        if(x>MAX_USERS-1) return myMovies[movieId].RatingAVG;
        if(movieId>MAX_MOVIES-1) return myUsers[x+1].RatingAVG;
        for(int i=0;i<MAX_FEATURES;i++)
            s+=MoviesFeatures[movieId][i]*UsersFeatures[x+1][i];

        if(s>5) s=5;
        if(s<1) s=1;
        return s;
    }
//================ FollowUp ~Stopping criterion
    void PJOB::FollowUp(int iter){
        //Randomly generated subset from the training set, maintained for the rest of the running.
        if(rank==MASTER) printf("[p%d]\tEstimating RMSE for stopping (%d)\n",rank,iter);
        int us, mv;
        check_local=myRatingCountM*CHECK;
        local_s=0;
        gloabal_s=0;
        if(iter==1){
            checkR.resize(myRatingCountM) ;
            iota(checkR.begin(), checkR.end(), 0);
            shuffle(checkR.begin(), checkR.end(), mt19937{random_device{}()});
        }

        //Followup on CHECK(%) of the total count:
        for(int i=0; i<check_local;i++){
            us=myRatingsM[checkR[i]].UserId; mv=myRatingsM[checkR[i]].MovieId;
            double p=0;
            for(int j=0;j<MAX_FEATURES;j++)
                p+=MoviesFeatures[mv][j]*UsersFeatures[us][j];
            local_s+=(myRatingsM[checkR[i]].Rating-p)*(myRatingsM[checkR[i]].Rating-p);
        }
        MPI_Reduce(&local_s,&gloabal_s,1,MPI_DOUBLE,MPI_SUM,MASTER,MPI_COMM_WORLD);
        MPI_Reduce(&check_local,&check_size,1,MPI_INT,MPI_SUM,MASTER,MPI_COMM_WORLD);

    }

//================ Testing performance with Test set: (For quick testing, divided the test set by 50)
    void PJOB::ALSTest(){
        FILE *testFile;
        double trueR, predictedR, Ssq=0, diff;
        Node R;
        ofstream prediction;

        //Seek the ratings from the test file:
        myTestcount=T_RATINGS/p/50;
        myStartTest=rank*myTestcount*sizeof(Node);
        // if(rank==p-1) myTestcount+=(T_RATINGS%p);
        try{ myTest=new Node[myTestcount];}
        catch (bad_alloc&) {printf("Memory Allocation Error\n"); exit(1);}
        testFile = fopen("test.bin","rb");
        printf("[p%d]\tLoading the test set\n",rank);
        if (testFile==NULL) {
            printf("File error, test.bin\n");
            exit(1);
        }
        fseek(testFile, myStartTest, SEEK_SET);
        fread(myTest, sizeof(Node), myTestcount, testFile);
        MPI_Barrier(MPI_COMM_WORLD);
        fclose(testFile);
        //Metadata for unknown users or movies:
        printf("[p%d]\tGathering the metadata\n",rank);
        int nM=(int) (MAX_MOVIES-1)/p,nU=(int) (MAX_USERS-1)/p;
        int* recvcountMetaU, * recvcountMetaM, * displsMetaM , * displsMetaU;
        recvcountMetaM=new int[p]; recvcountMetaU=new int[p]; displsMetaU=new int[p]; displsMetaM=new int[p];
        for (int i=0; i<p; i++) {
            recvcountMetaU[i]=nU;
            recvcountMetaM[i]=nM;
            displsMetaM[i]=i*nM;
            displsMetaU[i]=i*nU;
        }
        recvcountMetaM[p-1]+=((MAX_MOVIES-1)%p);
        recvcountMetaU[p-1]+=((MAX_USERS-1)%p);
        MPI_Allgatherv(&myMovies[myfirstMovie], recvcountMetaM[rank], mpi_Movie, &myMovies[1], recvcountMetaM, displsMetaM, mpi_Movie, MPI_COMM_WORLD);
        MPI_Allgatherv(&myUsers[myfirstUser], recvcountMetaU[rank], mpi_User, &myUsers[1], recvcountMetaU, displsMetaU, mpi_User, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        //The prediction
        printf("[p%d]\tStarting prediction:\n",rank);
        string filename="prediction_"+to_string(rank)+".txt";
        if(DoPrint) prediction.open(filename);
        for(int i=0; i<myTestcount; i++){
            R=myTest[i];
            trueR=(double) R.Rating;
            predictedR=PredictRating(R.MovieId, R.UserId);
            if(DoPrint){
                prediction<<"(M,U)=("<<R.MovieId<<","<<R.UserId<<")\tTR: "<<R.Rating<<"\tPR: "<<predictedR<<endl;
            }
            if(i%6000==0) printf("==%d%%",i*100/myTestcount);
            diff=trueR-predictedR;
            Ssq+=diff*diff;
        }
        printf("\n");
        SE_loc =Ssq;
        if(DoPrint) prediction.close();
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Reduce(&SE_loc,&SE_global,1,MPI_DOUBLE,MPI_SUM,MASTER,MPI_COMM_WORLD);
    }

//===================================//
//           MPI functions           //
//===================================//

//======== Brodcasting the indices and assigning users/movies:
    void jobBroadcast(PJOB* job) {
        int rank = job->rank, p = job->p;
        int firstMovie, firstUser;
        int numMovies=(int) (MAX_MOVIES-1)/p;
        int numUsers=(int) (MAX_USERS-1)/p;

        firstUser = rank*numUsers+1;
        firstMovie = rank*numMovies+1;

        if(rank==p-1){
            numMovies=MAX_MOVIES-1-(p-1)*numMovies;
            numUsers=MAX_USERS-1-(p-1)*numUsers;
        }

        MPI_Bcast (job->MoviesIndex, MAX_MOVIES, MPI_INT, MASTER, MPI_COMM_WORLD);
        MPI_Bcast (job->UsersIndex, MAX_USERS, MPI_INT, MASTER, MPI_COMM_WORLD);
        MPI_Bcast (job->flatMapping, MAX_USERS, MPI_INT, MASTER, MPI_COMM_WORLD);

        //Add the remaining to the last processor (p-1)
        if (rank==p-1)
            job->myRatingCountM = MAX_RATINGS-job->MoviesIndex[firstMovie];
        else
            job->myRatingCountM = job->MoviesIndex[firstMovie+numMovies]-job->MoviesIndex[firstMovie];

        if (rank==p-1)
            job->myRatingCountU = MAX_RATINGS-job->UsersIndex[firstUser];

        else
            job->myRatingCountU = job->UsersIndex[firstUser+numUsers]-job->UsersIndex[firstUser];

        job->myfirstUser = firstUser; job->myfirstMovie = firstMovie;
        job->numMovies = numMovies; job->numUsers = numUsers;
    }

//===================================//
//       Extra helping functions     //
//===================================//
    bool ReadNumber(char* BufferIn, int nLength, int &nPosition, char* BufferOut){
        int count = 0;
        int start = nPosition;
        char wc = 0;

        // looking for a number
        while (start < nLength)
        {
            wc = BufferIn[start];
            if ((wc >= 48 && wc <= 57) || (wc == 45)) break;
            start++;
        }

        nPosition = start;
        while (nPosition < nLength && ((wc >= 48 && wc <= 57) || wc == 69  || wc == 101 || wc == 45 || wc == 46))
        {
            BufferOut[count++] = wc;
            wc = BufferIn[++nPosition];
        }

        BufferOut[count] = 0;
        return (count > 0);
    }

    bool ParseInt(char* movieBuffer, int nLength, int &nPosition, int& nValue){
        char movieNumber[20];
        bool bResult = ReadNumber(movieBuffer, nLength, nPosition, movieNumber);
        nValue = (bResult) ? atoi(movieNumber) : 0;
        return bResult;
    }
    bool Usercomp(Node i, Node j) {
        return (i.UserId < j.UserId);
    }

    void pretty_print_matrix(double arr[][MAX_FEATURES], int firstRow, int lastRow, int firstColumn, int lastColumn){
        for(int i=firstRow; i<=lastRow;i++){
            printf("[%d]\t",i);
            for(int j=firstColumn; j<lastColumn;j++)
                printf("%.2f ",arr[i][j]);
            printf("\n");
        }
    }