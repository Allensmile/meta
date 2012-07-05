/**
 * @file postings.h
 */

#ifndef _POSTINGS_H_
#define _POSTINGS_H_

#include <map>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include "tokenizers/tokenizer.h"
#include "io/compressed_file_reader.h"
#include "io/compressed_file_writer.h"
#include "lexicon.h"
#include "structs.h"
#include "chunk_list.h"

using std::map;
using std::istringstream;
using std::ifstream;
using std::string;
using std::vector;
using std::cerr;
using std::endl;

/**
 * This is the interface to the large postings file located on disk.
 */
class Postings
{
    public:

        /**
         * Constructor; sets this Postings object to look at the specified file.
         */
        Postings(const string & postingsFile);

        /**
         * @param termData - used to determine the location of the term information in the postings
         * @return a vector of documents that contain the term the parameters refer to
         */
        vector<PostingData> getDocs(const TermData & termData) const;

        /**
         * @param termData - used to determine the location of the term information in the postings
         * @return a vector of documents that contain the term the parameters refer to
         */
        vector<PostingData> getCompressedDocs(const TermData & termData) const;

        /**
         * Creates lists of term information sorted by TermID on disk.
         * The lexicon is NOT updated in this function.
         * @param documents - a list of documents to index
         * @param chunkMBSize - the maximum size the postings chunks will be in
         *  memory before they're written to disk.
         * @param tokenizer - how to tokenize the indexed documents
         * @return the number of chunks created. Since their name is standard, they can easily
         *  be located.
         */
        size_t createChunks(vector<Document> & documents, size_t chunkMBSize, Tokenizer* tokenizer);

        /**
         * Creates the large postings file on disk out of many chunks.
         * @param chunks - a list of filenames indicating the location of the chunks to combine
         * @param lexicon - the lexicon to update while indexing
         */
        void createPostingsFile(size_t numChunks, Lexicon & lexicon);

        /**
         * Creates a compressed postings file on disk out of many chunks.
         * @param chunks - a list of filenames indicating the location of the chunks to combine
         * @param lexicon - the lexicon to update while indexing
         */
        void createCompressedPostingsFile(size_t numChunks, Lexicon & lexicon);

    private:

        string _postingsFilename;
        //CompressedFileReader _reader;
        InvertibleMap<DocID, string> _docMap;
        DocID _currentDocID;

        /**
         * @param pdata - list of PostingData for a term
         * @return the number of times a specific term has appeared in the corpus
         */
        unsigned int getTotalFreq(const vector<PostingData> & pdata) const;

        /**
         * Gets a line out of an uncompressed postings file.
         * This will be slow for large posting files and should
         *  only be used to check correctness.
         * @param lineNumber - which line number in the postings file to seek to
         * @return the contents of the specified line
         */
        string getLine(unsigned int lineNumber) const;

        /**
         * Writes a chunk to disk.
         * @param terms - the map of terms to write. It is cleared at the end of this function.
         * @param chunkNum - the number used for the filename
         */
        void writeChunk(map<TermID, vector<PostingData>> & terms, size_t chunkNum) const;

        /**
         * Keeps the DocID -> path mapping, and returns a new DocID
         *  if an unseen path is encountered.
         * @param path - the document path to check
         * @return the DocID for the given path
         */
        DocID getDocID(const string & path);
};

#endif
