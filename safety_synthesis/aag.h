#ifndef SYLVAN_AAG_H
#define SYLVAN_AAG_H

#include <sys/time.h>
#include <sys/mman.h>
#include <boost/graph/sloan_ordering.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <span>

typedef struct aag_header
{
    size_t m; // maximum variable index
    size_t i; // number of inputs
    size_t l; // number of latches
    size_t o; // number of outputs
    size_t a; // number of AND gates
    size_t b; // number of bad state properties
    size_t c; // number of invariant constraints
    size_t j; // number of justice properties
    size_t f; // number of fairness constraints
} aag_header_t;

typedef struct aag_file
{
    aag_header_t header;
    size_t *inputs;
    size_t *outputs;
    size_t *latches;
    size_t *l_next;
    int *lookup;
    size_t *gatelhs;
    size_t *gatelft;
    size_t *gatergt;
} aag_file_t;

typedef struct aag_buffer
{
    uint8_t *content;
    size_t size;
    size_t pos;
} aag_buffer_t;

int
aag_buffer_peek(aag_buffer_t *buffer)
{
    if (buffer->pos == buffer->size) return EOF;
    return (int) buffer->content[buffer->pos];
}

void
aag_buffer_skip(aag_buffer_t *buffer)
{
    buffer->pos++;
}

void
aag_buffer_read_wsnl(aag_buffer_t *buffer)
{
    while (true) {
        int c = aag_buffer_peek(buffer);
        if (c != ' ' && c != '\n' && c != '\t') return;
        aag_buffer_skip(buffer);
    }
}

void
aag_buffer_read_ws(aag_buffer_t *buffer)
{
    while (true) {
        int c = aag_buffer_peek(buffer);
        if (c != ' ' && c != '\t') return;
        aag_buffer_skip(buffer);
    }
}

void
aag_buffer_err()
{
    fprintf(stderr, "File read error.");
    exit(-1);
}

int
aag_buffer_read(aag_buffer_t *buffer)
{
    if (buffer->pos == buffer->size) return EOF;
    return (int) buffer->content[buffer->pos++];
}

void
aag_buffer_read_token(const char *str, aag_buffer_t *buffer)
{
    while (*str != 0) {
        if (aag_buffer_read(buffer) != (int) (uint8_t) (*str++)) {
            aag_buffer_err();
        }
    }
}

uint64_t
aag_buffer_read_uint(aag_buffer_t *buffer)
{
    uint64_t r = 0;
    while (true) {
        int c = aag_buffer_peek(buffer);
        if (c < '0' || c > '9') return r;
        r *= 10;
        r += c - '0';
        aag_buffer_skip(buffer);
    }
}

void
aag_buffer_read_string(std::string &s, aag_buffer_t *buffer)
{
    s = "";
    while (true) {
        int c = aag_buffer_peek(buffer);
        if (c == EOF || c == '\n') return;
        s += (char) c;
        aag_buffer_skip(buffer);
    }
}

void
aag_header_read(aag_header_t *header, aag_buffer_t *buffer)
{
    aag_buffer_read_wsnl(buffer);
    aag_buffer_read_token("aag", buffer);
    aag_buffer_read_ws(buffer);
    header->m = aag_buffer_read_uint(buffer);
    aag_buffer_read_ws(buffer);
    header->i = aag_buffer_read_uint(buffer);
    aag_buffer_read_ws(buffer);
    header->l = aag_buffer_read_uint(buffer);
    aag_buffer_read_ws(buffer);
    header->o = aag_buffer_read_uint(buffer);
    aag_buffer_read_ws(buffer);
    header->a = aag_buffer_read_uint(buffer);
    aag_buffer_read_ws(buffer);
    // optional
    header->b = 0;
    header->c = 0;
    header->j = 0;
    header->f = 0;
    aag_buffer_read_ws(buffer);
    if (aag_buffer_peek(buffer) != '\n') {
        header->b = aag_buffer_read_uint(buffer);
        aag_buffer_read_ws(buffer);
    }
    if (aag_buffer_peek(buffer) != '\n') {
        header->c = aag_buffer_read_uint(buffer);
        aag_buffer_read_ws(buffer);
    }
    if (aag_buffer_peek(buffer) != '\n') {
        header->j = aag_buffer_read_uint(buffer);
        aag_buffer_read_ws(buffer);
    }
    if (aag_buffer_peek(buffer) != '\n') {
        header->f = aag_buffer_read_uint(buffer);
    }
    aag_buffer_read_wsnl(buffer);

    if (header->o != 1) {
        fprintf(stderr, "expecting 1 output");
        exit(-1);
    }

    if (header->b != 0 or header->c != 0 or header->j != 0 or header->f != 0) {
        fprintf(stderr, "No support for new format.");
        exit(-1);
    }
}

void
aag_file_read(aag_file_t *aag, aag_buffer_t *buffer)
{
    aag_header_t header;
    aag_header_read(&header, buffer);

    aag->header = header;
    aag->inputs = new size_t[header.i];
    aag->latches = new size_t[header.l];
    aag->l_next = new size_t[header.l];
    aag->outputs = new size_t[header.o];
    aag->gatelhs = new size_t[header.a];
    aag->gatelft = new size_t[header.a];
    aag->gatergt = new size_t[header.a];
    aag->lookup = new int[header.m + 1];

    for (uint64_t i = 0; i < aag->header.i; i++) {
        aag->inputs[i] = aag_buffer_read_uint(buffer);
        aag_buffer_read_wsnl(buffer);
    }

    for (uint64_t l = 0; l < aag->header.l; l++) {
        aag->latches[l] = aag_buffer_read_uint(buffer);
        aag_buffer_read_ws(buffer);
        aag->l_next[l] = aag_buffer_read_uint(buffer);
        aag_buffer_read_wsnl(buffer);
    }

    for (uint64_t o = 0; o < aag->header.o; o++) {
        aag->outputs[o] = aag_buffer_read_uint(buffer);
        aag_buffer_read_wsnl(buffer);
    }

    for (uint64_t i = 0; i <= aag->header.m; i++) aag->lookup[i] = -1; // not an and-gate
    for (uint64_t a = 0; a < aag->header.a; a++) {
        aag->gatelhs[a] = aag_buffer_read_uint(buffer);
        aag->lookup[aag->gatelhs[a] / 2] = (int) a;
        aag_buffer_read_ws(buffer);
        aag->gatelft[a] = aag_buffer_read_uint(buffer);
        aag_buffer_read_ws(buffer);
        aag->gatergt[a] = aag_buffer_read_uint(buffer);
        aag_buffer_read_wsnl(buffer);
    }
}

#endif //SYLVAN_AAG_H
