#include "page.h"
#include "metadata.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <iomanip>

using namespace std;

const double factor = 0.85;
const double precision = 1e-7;

int main(int argc, char *argv[]) 
{
    ifstream metadata_read;
    metadata_read.open("metadata.txt");
    int page_counts, per_page_in_chunk;
    metadata_read >> page_counts;
    metadata_read >> per_page_in_chunk;
    Metadata *metadata = new Metadata(page_counts, per_page_in_chunk);

    // Read pages and get dangling pages
    std::vector<Page*> pages;
    std::vector<int> dangling_pages;
    std::vector<int> outlink_counts(page_counts);
    ifstream page_read;
    page_read.open("dataset.txt");
    for(int i = 0; i < metadata->get_page_counts(); i++) 
    {
        int page_id, page_outlink_counts, page_inlink_couts;
        page_read>>page_id>>page_outlink_counts>>page_inlink_couts;
        outlink_counts[page_id] = page_outlink_counts;
        if(page_outlink_counts == 0)
        {
            dangling_pages.push_back(page_id);
        }
        Page *page = new Page(page_id);
        page->set_in_link_counts(page_inlink_couts);
        page->add_out_link_counts(page_outlink_counts);
        for(int j = 0; j < page_inlink_couts; j++)
        {
            int ilink;
            page_read>>ilink;
            page->add_in_link(ilink);
        }
        pages.push_back(page);
    }

    std::vector<long double> cur_page_rank(page_counts);
	std::vector<long double> pre_page_rank(page_counts);
    for(int i = 0; i < page_counts; i++)
    {
        cur_page_rank[i] = 1.0 / page_counts;
    }

    int counts = 0;
	while (true) 
    {
		std::copy(cur_page_rank.begin(), cur_page_rank.end(), pre_page_rank.begin());
        long double new_rank = 0;
        for (int i = 0; i < page_counts; i++) 
        {
            new_rank = 0;
		    for (auto& page_from_id : pages[i] -> get_in_links()) 
            {
			    new_rank += pre_page_rank[page_from_id] / outlink_counts[page_from_id];
		    }
		    cur_page_rank[i] = new_rank;
        }

	    new_rank = 0;
	    for (auto& dangling_pade_id: dangling_pages) {
		    new_rank += pre_page_rank[dangling_pade_id];
	    }
	    for (auto cur_pr : cur_page_rank) 
        {
		    cur_pr += new_rank / page_counts;
	    }

        for (auto cur_pr : cur_page_rank) 
        {
		    cur_pr = factor * cur_pr + (1 - factor) / page_counts;
	    }

		long double err = 0;
        for (int i = 0; i < page_counts; i++) 
        {
		    err += std::abs(cur_page_rank[i] - pre_page_rank[i]);
        }
        cout<<err<<endl;

		if(err < precision) 
        {
            break;
        }
		++counts;
	}

    ofstream page_rank_output;
    page_rank_output.open("page_ranks.txt"); 
    for (int i = 0; i < page_counts; i++) 
    {
		page_rank_output<<setprecision(10)<<std::fixed<<cur_page_rank[i]<<endl;
	}

    return 0;
}