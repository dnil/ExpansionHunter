//
// Expansion Hunter
// Copyright (c) 2016 Illumina, Inc.
//
// Author: Egor Dolzhenko <edolzhenko@illumina.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "graphs/graph_mapping_operations.h"

#include <stdexcept>

using std::string;
using std::vector;

GraphMapping DecodeFromString(int32_t first_node_start,
                              const string& graph_cigar, const string& query,
                              const Graph& graph) {
  vector<int32_t> node_ids;
  vector<Mapping> node_mappings;
  int32_t query_pos = 0;
  string node_cigar;
  for (size_t index = 0; index != graph_cigar.length(); ++index) {
    node_cigar += graph_cigar[index];
    if (node_cigar.back() == ']') {
      string query_piece = query.substr((size_t)query_pos);
      int32_t ref_pos = node_mappings.empty() ? first_node_start : 0;

      string cigar;
      int32_t node_id;
      SplitNodeCigar(node_cigar, cigar, node_id);
      node_ids.push_back(node_id);
      const string& node_seq = graph.NodeSeq(node_id);
      Mapping node_mapping(ref_pos, cigar, query_piece, node_seq);
      node_mappings.push_back(node_mapping);
      query_pos += node_mapping.QuerySpan();
      node_cigar.clear();
    }
  }
  return GraphMapping(node_ids, node_mappings);
}

void SplitNodeCigar(const string& node_cigar, string& cigar, int32_t& node_id) {
  node_id = -1;
  string nodeid_encoding;
  for (size_t index = 0; index != node_cigar.length(); ++index) {
    if (node_cigar[index] == '[') {
      node_id = std::stoull(nodeid_encoding);
      cigar = node_cigar.substr(index + 1);
      cigar.pop_back();
      return;
    }
    if (isdigit(node_cigar[index]) == 0) {
      throw std::logic_error(node_cigar + " is a malformed node CIGAR");
    }
    nodeid_encoding += node_cigar[index];
  }
}

static string GetQuerySequence(const Operation& operation) {
  string encoding;
  switch (operation.type()) {
    case Operation::Type::kMatch:
    case Operation::Type::kMismatch:
      return operation.Query();
    default:
      return "";
  }
}

static string GetReferenceSequence(const Operation& operation) {
  string encoding;
  switch (operation.type()) {
    case Operation::Type::kMatch:
    case Operation::Type::kMismatch:
      return operation.Reference();
    default:
      return "";
  }
}

static string GetMatchPattern(const Operation& operation) {
  string encoding;
  switch (operation.type()) {
    case Operation::Type::kMatch:
      return string(operation.Length(), '|');
    case Operation::Type::kMismatch:
      return string(operation.Length(), ' ');
    default:
      return "";
  }
}

static string GetQuerySequence(const Mapping& mapping) {
  string encoding;
  for (size_t index = 0; index != mapping.NumOperations(); ++index) {
    const Operation& operation = mapping[index];
    encoding += GetQuerySequence(operation);
  }

  return encoding;
}

static string GetReferenceSequence(const Mapping& mapping) {
  string encoding;
  for (size_t index = 0; index != mapping.NumOperations(); ++index) {
    const Operation& operation = mapping[index];
    encoding += GetReferenceSequence(operation);
  }

  return encoding;
}

static string GetMatchPattern(const Mapping& mapping) {
  string encoding;
  for (size_t index = 0; index != mapping.NumOperations(); ++index) {
    const Operation& operation = mapping[index];
    encoding += GetMatchPattern(operation);
  }

  return encoding;
}

string EncodeGraphMapping(const GraphMapping& graph_mapping, int32_t padding) {
  string query_encoding, match_pattern, reference_encoding;

  const string padding_spaces(padding, ' ');

  for (const NodeMapping& node_mapping : graph_mapping) {
    if (query_encoding.empty()) {
      query_encoding = padding_spaces;
      match_pattern = padding_spaces;
      reference_encoding = padding_spaces;
    } else {
      query_encoding += '-';
      match_pattern += '-';
      reference_encoding += '-';
    }
    query_encoding += GetQuerySequence(node_mapping.mapping);
    match_pattern += GetMatchPattern(node_mapping.mapping);
    reference_encoding += GetReferenceSequence(node_mapping.mapping);
  }

  const string encoding =
      query_encoding + "\n" + match_pattern + "\n" + reference_encoding;
  return encoding;
}