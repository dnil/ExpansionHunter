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

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "graphs/graph_mapping.h"

namespace reads {

struct CoreInfo {
  std::string fragment_id;
  std::string bases;
  std::string quals;
};

struct SamInfo {
  int32_t chrom_id = -1;
  int32_t pos = -1;
  int32_t mapq = -1;
  int32_t mate_chrom_id = -1;
  int32_t mate_pos = -1;
  bool is_mapped = false;
  bool is_first_mate = false;
  bool is_mate_mapped = false;
};

struct GraphInfo {
  GraphMappingPtr canonical_mapping_ptr;
};

class Read {
 public:
  void SetCoreInfo(const std::string& fragment_id, const std::string& bases,
                   const std::string& quals);
  const std::string& FragmentId() const;
  const std::string& Bases() const;
  const std::string& Quals() const;

  // Provide access to information fro SAM files.
  int32_t SamChromId() const { return sam_info_.chrom_id; }
  void SetSamChromId(int32_t chrom_id) { sam_info_.chrom_id = chrom_id; }

  int32_t SamPos() const { return sam_info_.pos; }
  void SetSamPos(int32_t pos) { sam_info_.pos = pos; }

  int32_t SamMapq() const { return sam_info_.mapq; }
  void SetSamMapq(int32_t mapq) { sam_info_.mapq = mapq; }

  int32_t SamMateChromId() const { return sam_info_.mate_chrom_id; }
  void SetSamMateChromId(int32_t mate_chrom_id) {
    sam_info_.mate_chrom_id = mate_chrom_id;
  }

  int32_t SamMatePos() const { return sam_info_.mate_pos; }
  void SetSamMatePos(int32_t mate_pos) { sam_info_.mate_pos = mate_pos; }

  bool IsSamMapped() const { return sam_info_.is_mapped; }
  void SetIsSamMapped(bool is_mapped) { sam_info_.is_mapped = is_mapped; }

  bool IsFirstMate() const { return sam_info_.is_first_mate; }
  void SetIsFirstMate(bool is_first_mate) {
    sam_info_.is_first_mate = is_first_mate;
  }

  bool IsMateSamMapped() const { return sam_info_.is_mate_mapped; }
  void SetIsMateSamMapped(bool is_mate_mapped) {
    sam_info_.is_mate_mapped = is_mate_mapped;
  }

  // Provide access to graph-specific information.
  const GraphMapping& CanonicalMapping() const;
  void SetCanonicalMapping(GraphMappingPtr graph_mapping_ptr) {
    graph_info_.canonical_mapping_ptr = std::move(graph_mapping_ptr);
  }

  bool HasCanonicalMapping() const {
    return ((bool)graph_info_.canonical_mapping_ptr);
  }

 private:
  CoreInfo core_info_;
  SamInfo sam_info_;
  GraphInfo graph_info_;
};

typedef std::shared_ptr<Read> ReadPtr;

}  // namespace reads