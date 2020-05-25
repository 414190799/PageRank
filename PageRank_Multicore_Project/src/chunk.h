#ifndef CHUNK_H
#define CHUNK_H

#include "page.h"
#include<vector>

class Chunk {

private:
  int page_nums;
  std::vector<Page> pages;

public:
  Chunk();
  void add_page(Page page);
  void set_pages(std::vector<Page> pages);
  std::vector<Page> get_pages();
  int get_page_nums();

};

#endif