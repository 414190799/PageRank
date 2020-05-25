#ifndef PAGE_H
#define PAGE_H
#include<vector>

class Page {

private:
  int page_id;
  std::vector<int> in_links;
  int out_link_counts;
  int in_link_counts;

public:
  Page();
  Page(int id);
  int get_page_id();
  void set_page_id(int page_id);
  void add_in_link(int page_id);
  std::vector<int> get_in_links();
  int get_out_link_counts();
  void add_out_link_counts(int count);
  int get_in_link_counts();
  void set_in_link_counts(int inlink_counts);
};

#endif