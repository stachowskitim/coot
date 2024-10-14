#ifndef VALIDATION_INFORMATION_HH
#define VALIDATION_INFORMATION_HH

#include <vector>
#include "analysis/stats.hh"
#include "residue-validation-information.hh"

namespace coot {

   class chain_validation_information_t {
   public:
      std::string chain_id;
      std::vector<residue_validation_information_t> rviv;
      chain_validation_information_t() {}
      explicit chain_validation_information_t(const std::string &chain_id_in) : chain_id(chain_id_in) {}
      void add(const residue_validation_information_t &rvi) {
         add_residue_validation_information(rvi);
      }
      void add_residue_validation_information(const residue_validation_information_t &rvi) {
         rviv.push_back(rvi);
      }
   };

   class validation_information_min_max_t {
   public:
      bool is_set;
      double min;
      double max;
      validation_information_min_max_t() : is_set(false), min(0), max(0) {}
      validation_information_min_max_t(const double &min_in, const double &max_in) : is_set(true), min(min_in), max(max_in) {}
   };

#ifdef EMSCRIPTEN
#else
   // values for type:
   enum graph_data_type { UNSET, DENSITY, DISTORTION, ENERGY, PROBABILITY, CORRELATION, LOG_PROBABILITY, TORSION_ANGLE };
#endif

   class validation_information_t {
   public:
      std::string name;
      validation_information_min_max_t min_max;
      std::vector<chain_validation_information_t> cviv;

#ifdef EMSCRIPTEN
      std::string type;
      validation_information_t() : min_max(validation_information_min_max_t()), type("UNSET") {}
      validation_information_t(const std::string  &gdt, const validation_information_min_max_t &min_max_in) : min_max(min_max_in), type(gdt) {}
#else
      enum graph_data_type type;
      validation_information_t() : min_max(validation_information_min_max_t()), type(UNSET) {}
      validation_information_t(graph_data_type gdt, const validation_information_min_max_t &min_max_in) : min_max(min_max_in), type(gdt) {}
#endif

      unsigned int get_index_for_chain(const std::string &chain_id) {
         for (unsigned int i=0; i<cviv.size(); i++) {
            if (chain_id == cviv[i].chain_id)
               return i;
         }
         chain_validation_information_t cvi(chain_id);
         cviv.push_back(cvi);
         return cviv.size() -1;
      }
      void add_residue_validation_information(const residue_validation_information_t &rvi, const std::string &chain_id) {
         unsigned int idx = get_index_for_chain(chain_id);
         cviv[idx].add_residue_validation_information(rvi);
      }
      // one presumes that the chain id of this cvi does not match any current items in the cviv.
      void add(const chain_validation_information_t &cvi) {
         cviv.push_back(cvi);
      }
      //! return true of there are no data
      bool empty() const { return cviv.empty(); }
      //! set the min and max for the graph - internal use.
      void set_min_max() {
         unsigned int n = 0;
         double min =  9999999999999;
         double max = -9999999999999;
         for (const auto &chain : cviv) {
            for (const auto &res : chain.rviv) {
               n++;
               if (res.function_value < min) min = res.function_value;
               if (res.function_value > max) max = res.function_value;
            }
         }
         if (n > 0) {
            min_max.min = min;
         } else {
            min_max.max = max;
         }
      }

      //! get the stats for the data in this validation container
      stats::single get_stats() const {
         // 20230402-PE this function is in-line only because it is inconvenient at the moment to add
         // a new source file to the repo

         stats::single s;

         for (unsigned int i=0; i<cviv.size(); i++) {
            const auto &cvi = cviv[i];
            for (unsigned int j=0; j<cvi.rviv.size(); j++) {
               const auto &rvi = cvi.rviv[j];
               s.add(rvi.function_value);
            }
         }
         return s;
      }
   };

}

#endif // VALIDATION_INFORMATION_HH
