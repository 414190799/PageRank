#ifndef METADATA_H
#define METADATA_H


class Metadata {

private:
  int page_counts;
  int page_count_in_chunk;

public:
  Metadata(int count, int chunk_size);
  int get_page_counts();
  int get_page_count_in_chunk();

};

#endif