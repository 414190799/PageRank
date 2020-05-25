#include "page.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
using namespace std;

int main(int argc, char *argv[])
{

  int total_page_counts = atoi(argv[1]);
  int max_in_link_nums = atoi(argv[2]);
  int pages_per_chunk = atoi(argv[3]);

  vector<Page*> all_pages;
  for (int i = 0; i < total_page_counts; ++i)
  {
    all_pages.push_back(new Page(i));
  }

  for (int i = 0; i < total_page_counts; ++i) 
  {
    int in_link_nums = rand() % max_in_link_nums;
    for (int j = 0; j < in_link_nums; j++)
    {
      int in_link = rand() % total_page_counts;
      all_pages[i] -> add_in_link(in_link);
      all_pages[i] -> set_in_link_counts(in_link_nums);
      all_pages[in_link] -> add_out_link_counts(1);
    }
  }
  random_device rd;

  mt19937 prng(rd());
  
  std::shuffle(all_pages.begin(), all_pages.end(), prng);

  ofstream outdata;
  outdata.open("dataset.txt"); 
  if( !outdata ) {
    cerr << "Error: file could not be opened" << endl;
    exit(1);
  }
  for (int i = 0; i < total_page_counts; ++i)
  {
    Page *p = all_pages[i];
    outdata<<p -> get_page_id()<<" "<<p -> get_out_link_counts()<<" "<<p -> get_in_link_counts()<<endl;
    for (auto& in_link : p -> get_in_links())
    {
      outdata<<in_link<<" ";
    }
    outdata<<endl;
  }

  ofstream outdata2;
  outdata2.open("metadata.txt"); 
  if( !outdata2 ) {
    cerr << "Error: file could not be opened" << endl;
    exit(1);
  }
  outdata2<<total_page_counts<<" "<<pages_per_chunk<<endl;

  return 0;
}
