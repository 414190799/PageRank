#include "page.h"
#include<vector>

Page::Page() {
  page_id = 0;
  out_link_counts = 0;
}

Page::Page(int id) {
  page_id = id;
  out_link_counts = 0;
}


int Page::get_page_id() {
  return page_id;
}

void Page::set_page_id(int page_id) {
  this->page_id = page_id;
}

void Page::add_in_link(int page_id) {
  in_links.push_back(page_id);
}

std::vector<int> Page::get_in_links() {
  return in_links;
}

int Page::get_out_link_counts() {
  return out_link_counts;
}

void Page::add_out_link_counts(int count) {
  this->out_link_counts += count;
}

int Page::get_in_link_counts() {
  return in_link_counts;
}

void Page::set_in_link_counts(int inlink_counts) {
  this->in_link_counts = inlink_counts;
}