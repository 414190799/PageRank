#include "chunk.h"

Chunk::Chunk() {
  page_nums = 0;
}

void Chunk::add_page(Page page) {
  pages.push_back(page);
  ++page_nums;
}

void Chunk::set_pages(std::vector<Page> pages) {
  this->pages = pages;
  page_nums = pages.size();
}

int Chunk::get_page_nums() {
  return page_nums;
}