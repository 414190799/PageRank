#include "page.h"
#include "metadata.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <mpi.h>
#include <omp.h>

using namespace std;

const double factor = 0.85;
const double precision = 1e-7;
int THREAD;

int main(int argc, char *argv[]) 
{
    THREAD=atoi(argv[1]);
    MPI_Init(nullptr, nullptr);
    int comm_size, process_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
    
    int page_counts, per_page_in_chunk;
    if(process_rank == 0)
    {
        ifstream metadata_read;
        metadata_read.open("metadata.txt");
        metadata_read >> page_counts;
        metadata_read >> per_page_in_chunk;
        Metadata *metadata = new Metadata(page_counts, per_page_in_chunk);
    }

    std::vector<int> dangling_pages;
    std::vector<int> outlink_counts(page_counts);
    if(process_rank == 0)
    {
        ifstream page_rr;
        page_rr.open("dataset.txt");
        for(int i = 0; i < page_counts; i++)
        {
            int page_id, page_outlink_counts, page_inlink_couts;
            page_rr>>page_id>>page_outlink_counts>>page_inlink_couts;
            // cout<<page_id<<" "<<page_outlink_counts<<" "<<page_inlink_couts<<endl;
            outlink_counts[page_id] = page_outlink_counts;
            if(page_outlink_counts == 0)
            {
                dangling_pages.push_back(page_id);
            }
            page_rr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            page_rr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    // for(int i = 0; i < page_counts; i++) {
    //     cout<<outlink_counts[i]<<endl;
    // }

    MPI_Bcast(&page_counts, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&per_page_in_chunk, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(process_rank > 0)
    {
        outlink_counts.resize(page_counts);
    }
    MPI_Bcast(outlink_counts.data(), page_counts, MPI_INT, 0, MPI_COMM_WORLD);

    int work_size = page_counts / comm_size;
	int start = work_size * process_rank;
	int end = start + work_size;

    std::vector<Page*> pages;
    pages.resize(work_size);
    std:vector<Page*> global_pages;
    // Read pages and get dangling pages
    ifstream page_read;
    page_read.open("dataset.txt");

    for(int i = 0; i < page_counts; i++) 
    {
        int page_id, page_outlink_counts, page_inlink_couts;
        page_read>>page_id>>page_outlink_counts>>page_inlink_couts;
        Page *page = new Page(page_id);
        page->set_in_link_counts(page_inlink_couts);
        page->add_out_link_counts(page_outlink_counts);
        for(int j = 0; j < page_inlink_couts; j++)
        {
            int ilink;
            page_read>>ilink;
            page->add_in_link(ilink);
        }
        global_pages.push_back(page);
    }

    for(int i = start; i < end; i++) 
    {
        pages[i - start] = global_pages[i];
    }

    int remaining_pages_count = page_counts % comm_size;
	int remaining_start = page_counts - remaining_pages_count;

    if (process_rank == 0) 
    {
        // cout<<remaining_pages_count<<endl;
		if (remaining_pages_count > 0) 
        {
			for(int i = remaining_start; i < page_counts; i++) 
            {
                pages[i - start] = global_pages[i];
            }
		}
	}

    // if(process_rank == 1) {
    //     cout<<pages.size()<<endl;
    //     for(int i = 0; i < work_size; i++)
    //     {
    //         cout<<pages[i]->get_page_id()<<endl;
    //     }
    // }

    std::vector<long double> global_page_ranks(page_counts);
    std::vector<long double> cur_page_rank(pages.size());
    std::vector<long double> new_page_rank(page_counts);
	std::vector<long double> pre_page_rank;
    if(process_rank == 0)
    {
        for(int i = 0; i < page_counts; i++)
        {
            global_page_ranks[i] = 1.0 / page_counts;
        }
        pre_page_rank.resize(page_counts);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    int counts = 0;
    bool flag = true;
	while (flag) 
    {
        MPI_Bcast(global_page_ranks.data(), page_counts, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
        // cout<<"calculating..."<<endl;
        if(process_rank == 0)
        {
            std::copy(global_page_ranks.begin(), global_page_ranks.end(), pre_page_rank.begin());
        }
		
        #pragma omp parallel for num_threads(THREAD)
        for (int i = 0; i < pages.size(); i++) 
        {
            new_page_rank[i]=0;
		    for (auto page_from_id : pages[i] -> get_in_links()) 
            {
			    new_page_rank[i] += global_page_ranks[page_from_id] / outlink_counts[page_from_id];
		    }
            // cout<<new_rank<<endl;
		    cur_page_rank[i] = new_page_rank[i];
        }

        MPI_Gather(cur_page_rank.data(), work_size, MPI_LONG_DOUBLE,
				global_page_ranks.data(), work_size, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);

        // cout<<"111"<<endl;

        if(process_rank == 0)
        {
            // cout<<"calculating..."<<endl;
            std::copy(cur_page_rank.begin() + work_size, cur_page_rank.end(),
					global_page_ranks.begin() + remaining_start);
	        long double new_rank = 0;
            #pragma omp parallel for reduction(+:new_rank) num_threads(THREAD)
	        for (auto dangling_pade_id: dangling_pages) {
		        new_rank += pre_page_rank[dangling_pade_id];
	        }
	        for (auto cur_pr : global_page_ranks) 
            {
		        cur_pr += new_rank / page_counts;
	        }

            for (auto cur_pr : global_page_ranks) 
            {
		        cur_pr = factor * cur_pr + (1 - factor) / page_counts;
	        }

		    long double err = 0;
            #pragma omp parallel for reduction(+:err) num_threads(THREAD)
            for (int i = 0; i < page_counts; i++) 
            {
		        err += std::abs(global_page_ranks[i] - pre_page_rank[i]);
            }
            // cout<<err<<endl;

		    flag = err > precision;
        }
        MPI_Bcast(&flag, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
		++counts;
	}
    MPI_Barrier(MPI_COMM_WORLD);
    if(process_rank == 0)
    {
        ofstream page_rank_output;
        page_rank_output.open("page_ranks.txt"); 
        for (int i = 0; i < page_counts; i++) 
        {
		    page_rank_output<<setprecision(10)<<std::fixed<<cur_page_rank[i]<<endl;
	    }
    }
    MPI_Finalize();
    return 0;
}