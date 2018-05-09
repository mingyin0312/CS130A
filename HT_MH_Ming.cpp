//
//  main.cpp
//  HT_MH
//
//  Created by Ming Yin on 5/4/18.
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

//class Node used to store information about each node
class Node{
    
public:
    int key, value, priority;
    Node(int key,int value,int priority=0);

public:
    //These pointers are used to save the parent and children of the max heap
    Node* parent;
    Node* lchild;
    Node* rchild;
    
};


Node::Node(int key, int value, int priority){
    this->key=key;
    this->value=value;
    this->priority=priority;
    parent=0;
    lchild=0;
    rchild=0;

}


//swap is used to swap the information about of pointers
void swap(Node* &s,Node* &t){
    
    Node temp(s->key,s->value,s->priority);
    
    s->key=t->key;
    s->value=t->value;
    s->priority=t->priority;
    t->key=temp.key;
    t->value=temp.value;
    t->priority=temp.priority;
    
}

//max_heapify is used to maintain the structure of max heap
void max_heapify(vector<Node*> &p, int loc){
    
    Node* s;
    
    s=p[loc];//loc is the specified location that need to be heapify

    while(s->parent!=NULL){
    
        if(s->priority>(s->parent)->priority)
        {
            swap(s,s->parent);
            
            s=s->parent;
            
            continue;
        }
        
        else if((s->priority==(s->parent)->priority)&&(s->key >(s->parent)->key)){
    
            swap(s,s->parent);
           
            s=s->parent;
            
            continue;
        }
        break;
    }
       
}


//insert a single node "new_node" to the existence heap "p"
void single_point_insert_heap(Node* &new_node, vector<Node*> &p){
   
    if((int)p.size()>0){
        
        for(int i=0; i<(int)p.size(); i++){
            //if one of the child is Null, then this is the next avaiable spot that keep the complete tree structure
            if(p[i]->lchild==NULL)
            {
                p[i]->lchild=new_node;
                new_node->parent=p[i];
                p.push_back(new_node);
                break;
            }
            
            if(p[i]->rchild==NULL)
            {
                p[i]->rchild=new_node;
                new_node->parent=p[i];
                p.push_back(new_node);
                break;
            }
        }
    }
    
    else{
        
        p.push_back(new_node);
    }
    new_node=NULL;
    delete new_node;
    
    //after insertion, heapify the heap
    if(p.size()>1){
       max_heapify(p,(int)p.size()-1);
    }
}


//use to delete a node
Node* deleteMax(vector <Node*> &p){
    
    swap(p[0],p[p.size()-1]);
    
    //delete the child pointer of the parent node of the deleted node
    if((int)p.size()>1){
        
        if(p[p.size()-1]==p[p.size()-1]->parent->lchild)
            
            p[p.size()-1]->parent->lchild=NULL;
        
        else if(p[p.size()-1]==p[p.size()-1]->parent->rchild)
            
            p[p.size()-1]->parent->rchild=NULL;
    }
    
    Node*a=new Node(p[p.size()-1]->key,p[p.size()-1]->value,p[p.size()-1]->priority);
    
    p.pop_back();//delete the node
    
    for(int i=0;i<p.size();i++){
        
        max_heapify(p,i);
    }
    return a;//return the node deleted from the max heap, and will be inserted to hash table later
    
}


// find the location of hash function and insert it into hash table "q"
void find_location_hash(Node* &b, vector <Node*> &q){
    int c[q.size()];
    int count=0;
    for(int i=0;i<q.size();i++)
        c[i]=0;
    int i=(b->key)%q.size();
    while(count==0){
        if(!q[i]){
            q[i]=b;
            break;
        }
        else{
            c[i]++;
            i=(i+1)%q.size();
            count=c[i];
    
        }
    }
}

//first time load the k elements with highest priorities to the hash table "q" from max heap "p"
void Load_HT_initial(vector <Node*> &p,vector <Node*> &q, int k){
    
    Node *b;
    
    for(int i=0; i<k; i++){
        
        b=deleteMax(p);
        
        find_location_hash(b, q);
        
    }
}


//load the Node "b" into HT "q" if priority the higher the minmum priority of MH "p",
//otherwise insert it back to max heap
void single_Load_HT(Node*&b,vector <Node*> &p,vector <Node*> &q ){
    
    int min=0;
    
    for(int i=0; i<q.size(); i++){
        
        if((q[min]->priority>q[i]->priority)||((q[min]->priority==q[i]->priority)&&(q[min]->key>q[i]->key)))
            min=i;
        
    }
    
    if((b->priority>q[min]->priority)||((b->priority==q[min]->priority)&&(b->key>q[min]->key)))
        
        swap(b,q[min]);

    single_point_insert_heap(b, p);
}


//load the first maximum priority elements into HT "q"
void Load_HT(vector <Node*> &p,vector <Node*> &q){
    for(int i=0;i<p.size();i++){
        max_heapify(p,i);
    }
  
    for(int i=0;i<q.size();i++){
        Node * a=new Node(0,0);
        a=deleteMax(p);
        
        single_Load_HT(a, p, q);
        
    }
    
}


//linear probing to search the element in the hash table, return the index if exists
int Linear_prob_HT(int& Key, vector<Node*>&q){
    
    int f=Key%q.size();
    
    for(int j=0;j<q.size();j++)
    {
        if(q[f]->key==Key)
            return f;
        f=(f+1)%q.size();
    }
    return -1;
}

//search in the max heap, return the index if exists
int search_MH(int& Key,vector<Node*>&p){
    int f=Key%p.size();
    for(int j=0;j<p.size();j++)
    {
        if(p[f]->key==Key)
            return f;
        f=(f+1)%p.size();
    }
    return -1;
}


//search query
void Search(int& Key,vector<Node*>&p,vector<Node*>&q, int& sign ,int& value){
    if(Linear_prob_HT(Key, q)!=-1){
        q[Linear_prob_HT(Key, q)]->priority+=1;
        value=q[Linear_prob_HT(Key, q)]->value;
        sign=0;
    }
    else if(search_MH(Key, p)!=-1){
            p[search_MH(Key, p)]->priority+=1;
            value=p[search_MH(Key, p)]->value;
            sign=1;
        }
    else{
        sign=-1;
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
            
                temp_line.push_back(stoi(it->str()));
            //store it into a one dimension vector
        }
        a.push_back(temp_line);  //keep the data into vector<vector<int>> a
        temp_line.clear();
    }
    
    in.close();//finish reading the file
    
    ofstream out("HT_MH_output.txt");// save data to the output
    
   
    
    int number=a[0][0];//number of cases to be tested
    
    int i=1;//count the number to be performed
    
    //if the number of cases to be tested if not reduce to 0, use while loop again
    while(number--){
        
        int k=a[i][0];//size of hash_table

        i++;
        if(i>=a.size())
            break;
        
        //in each test case, create p for max heap and q for hash table
        vector<Node*> p;
        
        vector<Node*> q(k);
        
        int input=a[i][0];//denote the size of input

        i++;
        if(i>=a.size())
            break;
        
        int query=a[i][0];//number of queries
        
        i++;
        if(i>=a.size())
            break;
        
        //Load_MH operation
        for(int j=0;j<input;j++){
            
            Node* b=new Node(a[i][0],a[i][1]);
            
            single_point_insert_heap(b,p);
            
            b=NULL;
            
            delete b;
            
            i++;
        }
        
        //first time load hash table
        Load_HT_initial(p, q, k);
   
    
        //queries
        for(int j=0;j<query;j++)
        {
            switch (a[i][0]) {
                    
                //corresponds to the operation load_HT
                case 1:
                    Load_HT(p, q);
                    break;
                    
                //Display operation
                case 3:
                {
                    for(int c=0; c<q.size(); c++)
                        
                        out<<q[c]->key<<" ";
                    
                    out<<endl;
                    
                    for(int c=0;c<p.size();c++)
                        
                        out<<p[c]->key<<" ";
                    
                    out<<endl;

                    break;
                    
                }
                  
                //Corresponds to access key operation
                case 2:
                {
                    
                    int sign;//use to return 0,1,-1
                    
                    int value;//to store the value that get from the query
                    
                    Search(a[i][1], p, q, sign,value);//perform search
                    
                    //output the result
                    if(sign==-1)
                        out<<sign<<endl;
                    else
                        out<<sign<<" "<<value<<endl;
           
                    break;
                }
                
            }
            i++;
        }
        
        
        //after the queries of each test case, free the pointers to avoid memory leak
        for(int j=0;j<p.size();j++)
            delete p[j];
        
        for(int j=0;j<q.size();j++)
            delete q[j];
        
        out<<endl;
        
        //end the while loop if the index is greater than the size of input vector "a"
        if(i>=a.size())
            break;
        
    }
    
    return 0;
    
}







