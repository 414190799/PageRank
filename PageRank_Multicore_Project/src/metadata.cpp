#include "metadata.h"

Metadata::Metadata(int count, int chunk_size) {
  page_counts = count;
  page_count_in_chunk = chunk_size;
}

int Metadata::get_page_counts() {
  return page_counts;
}

int Metadata::get_page_count_in_chunk() {
  return page_count_in_chunk;
}