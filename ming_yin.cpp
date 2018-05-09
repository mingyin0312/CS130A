//
//  main.cpp
//  Perfect_hashing
//
//  Created by Ming Yin 4703872 on 4/22/18.
//  Copyright © 2018 Ming Yin. All rights reserved.
//

#include <time.h>
#include <vector>
#include <fstream>
#include <regex>
#include <string>
#include<stdlib.h>
#include<cmath>
#include <iostream>
using namespace std;

//class Matrix is used for perform matrix calculation
class Matrix{
public:
    Matrix(int row, int column);

    ~Matrix();
    
    void resetSize(int n,int m);//change the size of matrix
    
    void resetMatrix();//regenerate the random matrix
    
    void printMatrix() const;// you can this to print matrix if you want
    
    int &element(int i, int j){return elements[i*row+j];} //get element(i,j)
    
    int element(int i, int j) const {return elements[i*row+j];} //overloading for constant inputs
    
    friend vector<int> & MatrixMultiplication(Matrix& h, vector<int>& x);//Matrix Multiplication
    
    int row,column;// number of row and column of the matrix
    
    int *elements; //pointer to save the first address of element of matrix
    
};

Matrix::Matrix(int row, int column):row(row),column(column){
    elements=new int[row*column];
    srand((unsigned)time(0));
    for(int i=0;i<row*column;i++){
        elements[i]=(rand()%2);
    }
}

void Matrix::resetSize(int n,int m){
    row=n;
    column=m;
    delete[] elements;
    elements=new int[row*column];
    for(int i=0;i<row*column;i++){
        elements[i]=(rand()%2);
    }

}

Matrix::~Matrix(){
    delete[] elements;
}


void Matrix::resetMatrix(){
    for(int i=0;i<row*column;i++){
        elements[i]=(rand()%2);
    }
}


void Matrix::printMatrix() const{
    for(int i=0;i<row;i++){
        for(int j=0;j<column;j++)
            cout<<element(i,j);
        cout<<endl;
    }
}

vector<int> & MatrixMultiplication(Matrix& h, vector<int>& x){
    vector<int> *a=new vector<int>;
    int sum=0;
    for (int i=0;i<h.row;i++)
    { for(int j=0;j<h.column;j++)
            sum+=h.element(i,j)*x[j];
        a->push_back(sum%2);
        sum=0;
    }
    return *a;
}

//Convert binary number to decimal number
int dec(vector<int> & a){
    int s=0;
    for(int i=(int)(a.size());i>0;i--){
        if(a[a.size()-i]==0)
            continue;
        s+=a[a.size()-i]*pow(2,i-1);
    }
    return s;
}

//class Bucket is used for record the buckets after the first level hashing
class Bucket{
public:
    
    int indicator;//is used to indicate if we need to reset the size of hash table
    
    vector<vector<int>> array;//is used to store the data that has been hashed to this bucket
    
    int count;//count number of data in this bucket,i.e., the size of array
    
    Matrix H;// represent the hash matrix for second level hashing
    
    Bucket(int row=0,int column=0,int count=0,int in=0):count(count),H(row,column),indicator(in){};
    
    void load_data(vector<int> &a);//is used to load data into array
    
    bool second_level_hash();//perform second level hashing for one time, return TRUE for no collsion and FALSE for collsions
    
};

void Bucket::load_data(vector<int> &a){
    array.push_back(a);
}

bool Bucket::second_level_hash(){
    
    int m=count*count;//m is the number of buckets for second level hashing
    
    //if no data or just one data in this bucket, no need to do second level hashing
    if(m<=1)
        return true;
    
    //otherwise do second level hashing
    
    int r[m];//denote the number of data hashed to this bucket
    
    for(int i=0;i<m;i++)
        r[i]=0;
    
    //if do the second level hashing for the first time, then need reset size, otherwise just reset the elements in matrix
    if(indicator==0)
        H.resetSize(ceil(log2(m)),(int)array[0].size());
    else
        H.resetMatrix();

    indicator++;
    
    //hash data to certain bucket, r is used to record to the number hashed to this bucket
    for(int i=0;i<count;i++){
        vector<int> s=MatrixMultiplication(H, array[i]);
        r[dec(s)%m]++;
    }
    
    //if more than 1 element hash to the same bucket, then there is collision, return false, otherwise return true
    for(int i=0;i<m;i++)
    {
        if(r[i]>1)
            return false;
    }
    return true;
}



//First level hashing
void first_level_hash(Matrix &h, vector<vector<int>> &a,int n,Bucket*buck){
    vector<int>b;
    vector<int>c;
    //put data into corresponding buckets
    for(int i=0;i<n;i++){
        for(int j=0;j<a[0].size();j++)
            b.push_back(a[i][j]);
        c=MatrixMultiplication(h, b);
        int e=dec(c)%n;
        (buck+e)->load_data(b);
        (buck+e)->count++;
        b.clear();
        c.clear();
    }
}

//decide if hashing satisfies requirement, i.e., if total storage is less then 4*n
bool Rehashing(int m, Bucket *buck){
    int space=0;
    for(int i=0;i<m;i++){
        if(((buck+i)->count)>1)
            space+=((buck+i)->count)*((buck+i)->count)+1;
        else
            space+=1;
    }
    return (space<2*m);
        
}

//return the total space used for this specific hashing
int Rehashing1(int m, Bucket *buck){
    int space=0;
    for(int i=0;i<m;i++){
        if(((buck+i)->count)>1)
            space+=((buck+i)->count)*((buck+i)->count)+1;
        else
            space+=1;
    }
    return space;
}

//reset buckets
void resetBuck(int m,Bucket*buck){
    for(int i=0;i<m;i++){
        (buck+i)->count=0;
        (buck+i)->array.clear();
    }
}


int main(){
   
    //read the data from .txt file into vector<vector<int>> a
    vector<int> temp_line;
    vector<vector<int>> a;
    string line;
    string stv;
    cout<<"Type the name of the input file: "<<endl;
    cin>>stv;
    ifstream in(stv);  //read file
    regex pat_regex("[[:digit:]]+");  //matching principle
    
    while(getline(in, line)) {  //read by row
        for (sregex_iterator it(line.begin(), line.end(), pat_regex), end_it; it != end_it; ++it) {  //matching all the characters in a line
            for(int i=0;i<(it->str()).length();i++){
                temp_line.push_back((int)((it->str())[i])-48);
            }//store it into a one dimension vector
        }
        a.push_back(temp_line);  //keep the data into vector<vector<int>> a
        temp_line.clear();
    }
    
    in.close();//finish reading the file
    
    
    int m=(int)a.size(); //get the number of rows of inputs
    int l=(int)a[0].size(); //get the number of columns of inputs
    int space; //denote the total space used
    
    
    Matrix mat(ceil(log2(m)),l);// hasing matrix for the first level
   
    Bucket buck[m]; // buckets used to save the results of first level hashing
    
    do{
        mat.resetMatrix(); //generate hashing matrix every time if condition not satisfied
        resetBuck(m, buck); //clear buckets every time before first level hashing
        first_level_hash(mat,a,m,buck);// perform first level hashing
    }while(!Rehashing(m,buck)); //check conditions, if not satisfied, do it again
    
    space=Rehashing1(m,buck); //save the total space used for hashing of these two levels
    
    for(int i=0;i<m;i++){
        
        while(!buck[i].second_level_hash()){}//perform second level hashing for each buckets
        
    }
    
    
    ofstream out("Perfect_Hashing_output.txt");// save data to the output
    
    out<<"The first level hash matrix is:"<<endl;
    
    out<<endl;
    
    //save the first level hashing matrix
    for(int i=0;i<mat.row;i++){
        for(int j=0;j<mat.column;j++)
            out<<mat.element(i,j);
        out<<endl;
    }
    out<<endl;
    
    out<<"The second level hash matrix is: "<<endl;
    
    out<<endl;
    
    //save the second level hashing matrices
    //if no need to do second level hashing, the matrix is Null
    for(int i=0;i<m;i++){
    
         if(buck[i].count>1)
         {
             for(int s=0;s<buck[i].H.row;s++){
                 for(int j=0;j<buck[i].H.column;j++)
                     out<<mat.element(s,j);
                 out<<endl;
             }
         out<<endl;
         }
       else
           out<<"NULL"<<endl;
       
       out<<endl;
    
    }
    
    out<<"The total space used:"<<space<<endl;
    
    out<<endl;
    
    //save the lookup accesses required for the elements in the given order.
    out<<"Number of visit needed for every data:"<<endl;
    
    out<<endl;
    
    for(int i=0;i<m;i++){
        out<<"Number of visit needed for "<<i+1<<"-th data: ";
        int e=dec(MatrixMultiplication(mat, a[i]))%m;
        if(buck[e].count>1)
            out<<2<<endl;
        else
            out<<1<<endl;
        out<<endl;
    }
    
    //close file
    out.close();
    
    return 0;
}





