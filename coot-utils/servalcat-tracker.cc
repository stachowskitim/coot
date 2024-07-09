
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "json.hpp"
using json = nlohmann::json;

class point_t {
public:
   point_t(double x, double y) : x(x), y(y) {}
   explicit point_t(std::pair<double, double> &xy) : x(xy.first), y(xy.second) {}
   explicit point_t(std::pair<double, double> &xy, double x_scale, double y_scale, double x_offset, double y_offset) :
      x(x_offset + x_scale * xy.first), y(y_offset + y_scale * xy.second) {}
   double x;
   double y;
   point_t canvas_convert() const {
      double new_x = x;
      double new_y = -y + 100;
      return point_t(new_x, new_y);
   }
};

class summary_data_t {
public:
   summary_data_t() : cycle_number(0), fsc_average(0), mll(0) {}
   summary_data_t(int c, double f, double m) : cycle_number(c), fsc_average(f), mll(m) {}
   int cycle_number;
   double fsc_average;
   double mll;
};

std::string make_line(const point_t &p1, const point_t &p2, const std::string &colour, double width,
                      bool dashed, bool do_arrow_head) {

   point_t p1c = p1.canvas_convert();
   point_t p2c = p2.canvas_convert();

   do_arrow_head = false;

   std::string line;
   line += "   <line x1=\"";
   line += std::to_string(p1c.x);
   line += "\" y1=\"";
   line += std::to_string(p1c.y);
   line += "\" x2=\"";
   line += std::to_string(p2c.x);
   line += "\" y2=\"";
   line += std::to_string(p2c.y);
   line += "\" " ;
   line += "style=\"stroke:";
   line += colour;
   line += ";stroke-width:";
   line += std::to_string(width);
   line += "\"";
   if (dashed)
      line += " stroke-dasharray=\"1\"";
   if (do_arrow_head)
      // line += " marker-end=\"url(#arrow)\"";
      line += " marker-end=url(#arrow)";
   line += " />\n";

   return line;
}

class summary_data_container_t {
public:
   summary_data_container_t() { init(); }
   std::vector<summary_data_t> data;
   double x_scale;
   double y_scale;
   double x_offset;
   double y_offset;

   void init() {
      x_scale = 10.0;
      y_scale = 80.0;
      x_offset = 22.0;
      y_offset = 80.0; // move it down the page
   }
   void add(const summary_data_t &d) { data.push_back(d); }
   void output() {

      std::string output_file_name = "summary.svg";
      std::string s = make_complete_svg();
      std::ofstream f(output_file_name);
      f << s;
      f.close();
   }

   std::string make_complete_svg() {

      std::string s;
      std::string svg_header_1 = "<svg xmlns=\"http://www.w3.org/2000/svg\"\n   xmlns:xlink=\"http://www.w3.org/1999/xlink\" ";
      std::string svg_header_2 = ">\n";

      std::string svg_footer = "</svg>\n";

      double min_x = -10;
      double min_y = -10;
      double max_x = 400;
      double max_y = 200;

      std::string viewBox_string = "viewBox=" + std::string("\"") +
         std::to_string(min_x) + std::string(" ") +
         std::to_string(min_y) + std::string(" ") +
         std::to_string(max_x) + std::string(" ") +
         std::to_string(max_y) + std::string("\"");

      s += svg_header_1;
      s += viewBox_string;
      s += svg_header_2;
      s += graph_internals("fsc_average");

      x_offset += 140;
      s += graph_internals("mll");
      s += svg_footer;
      return s;
   }

   std::vector<std::string> make_grid_lines(unsigned int n_cycles) const {

      unsigned int n_lines = 5;
      if (n_cycles > n_lines) n_lines = n_cycles;

      std::vector<std::string> v;
      for (unsigned int i=1; i<=n_lines; i++) { // not at x = 0;
         std::pair<double, double> xy_1(i, 0.0);
         std::pair<double, double> xy_2(i, 1.2);
         point_t pt_1(xy_1, x_scale, y_scale, x_offset, y_offset);
         point_t pt_2(xy_2, x_scale, y_scale, x_offset, y_offset);
         std::string l = make_line(pt_1, pt_2, "#aaaaaa", 0.4, true, false);
         v.push_back(l);
      }
      return v;
   }
   
   std::vector<std::string> make_graph_lines(const std::string &type) const {

      // convert this to polylines

      std::cout << "make_graph_lines() " << type << std::endl;

      std::string colour = "#662222";
      std::vector<std::string> v;

      if (type == "fsc_average") {
         double this_graph_scale = 1.2;
         std::vector<std::pair<double, double> > data_points;
         for (unsigned int i=0; i<data.size(); i++)
            data_points.push_back(std::make_pair(static_cast<double>(i), data[i].fsc_average));
         for (unsigned int i=0; i<data.size(); i++) {
            if (i < (data.size() - 1)) {
               point_t p1(data_points[i  ], x_scale, this_graph_scale * y_scale, x_offset, y_offset);
               point_t p2(data_points[i+1], x_scale, this_graph_scale * y_scale, x_offset, y_offset);
               std::string line = make_line(p1, p2, colour, 0.5, false, false);
               v.push_back(line);
            }
         }
      }

      if (type == "mll") {
         colour = "#222299";
         double y_scale_mll = 0.00000024;
         std::vector<std::pair<double, double> > data_points;
         for (unsigned int i=0; i<data.size(); i++)
            data_points.push_back(std::make_pair(static_cast<double>(i), data[i].mll));
         for (unsigned int i=0; i<data.size(); i++) {
            if (i < (data.size() - 1)) {
               // std::cout << "mll data_points: " << data_points[i].first << "  " << data_points[i].second << std::endl;
               point_t p1(data_points[i  ], x_scale, y_scale * y_scale_mll, x_offset, y_offset);
               point_t p2(data_points[i+1], x_scale, y_scale * y_scale_mll, x_offset, y_offset);
               std::string line = make_line(p1, p2, colour, 0.5, false, false);
               v.push_back(line);
            }
         }
      }
      return v;
   }

   std::string make_x_axis_label(const std::string &x_axis_text) const {

      point_t p(40 + x_offset, y_offset - 14);
      point_t pc = p.canvas_convert();
      std::string s = "   <text font-family=\"Helvetica, sans-serif\" font-size=\"5\" x=\"";
      s += std::to_string(pc.x);
      s += "\" y=\"";
      s += std::to_string(pc.y);
      s += "\">";
      s += x_axis_text;
      s += "</text>\n";
      return s;
   }

   std::string make_y_axis_label(const std::string &y_axis_text) const {
      point_t p(-27 + x_offset, 50 + y_offset);
      point_t pc = p.canvas_convert();
      std::string s = "   <text font-family=\"Helvetica, sans-serif\" font-size=\"5\" x=\"";
      s += std::to_string(pc.x);
      s += "\" y=\"";
      s += std::to_string(pc.y);
      s += "\">";
      s += y_axis_text;
      s += "</text>\n";
      return s;
   }

   std::string make_tick_marks_x_axis(unsigned int n_cycles) const {
      std::string s;
      unsigned int n_lines = 5;
      if (n_cycles > n_lines) n_lines = n_cycles;
      for (unsigned int i=1; i<=n_lines; i++) { // not at x = 0;
         std::pair<double, double> xy_1(i, 0.0);
         std::pair<double, double> xy_2(i, -0.03);
         point_t pt_1(xy_1, x_scale, y_scale, x_offset, y_offset);
         point_t pt_2(xy_2, x_scale, y_scale, x_offset, y_offset);
         std::string l = make_line(pt_1, pt_2, "#222222", 0.4, false, false);
         s += l;
      }
      return s;
   }

   std::string make_tick_marks_y_axis() const {
      std::string s = "   \n";
      unsigned int n_ticks = 5;
      for (unsigned int i=0; i<=n_ticks; i++) {
         double f = static_cast<double>(i) / static_cast<double>(n_ticks);
         double y = 1.2 * f;
         std::pair<double, double> xy_1(0.0,  y);
         std::pair<double, double> xy_2(-0.3, y);
         point_t pt_1(xy_1, x_scale, y_scale, x_offset, y_offset);
         point_t pt_2(xy_2, x_scale, y_scale, x_offset, y_offset);
         std::string l = make_line(pt_1, pt_2, "#222222", 0.4, false, false);
         s += l;
      }
      return s;
   }

   std::string make_x_axis_tick_labels(unsigned int n_cycles, double x_scale, double x_offset) const {
      std::string s;
      unsigned int n_lines = 5;
      if (n_cycles > n_lines) n_lines = n_cycles;
      for (unsigned int i=1; i<=n_lines; i++) { // not at x = 0;
         point_t p( x_scale * i + x_offset - 1.0, y_offset - 6);
         point_t pc = p.canvas_convert();
         std::string l = "   <text font-family=\"Helvetica, sans-serif\" font-size=\"3\" ";
         l += "x=\"";
         l += std::to_string(pc.x);
         l += "\" y=\"";
         l += std::to_string(pc.y);
         l += "\">";
         l += std::to_string(i);
         l += "</text>\n";
         s += l;
      }
      return s;
   }

   std::string double_to_string(const double &d, unsigned int n_dec_pl) const {
      std::stringstream s;
      s << std::right << std::fixed;
      s << std::setprecision(n_dec_pl);
      s << d;
      std::string ss = s.str();
      return ss;
   }

   std::string make_y_axis_tick_labels(const std::string &graph_type, double x_scale, double x_offset) const {
      std::string s;
      unsigned int n_lines = 5;
      for (unsigned int i=0; i<=n_lines; i++) { // not at x = 0;
         double f = static_cast<double>(i) / static_cast<double>(n_lines);
         double v = 0.0;
         double y = 0.0;
         double tw_offset = 12;
         std::string value_as_string;
         if (graph_type == "fsc_average") {
            tw_offset = 12;
            v = f;
            y = 96.0 * f; // 100 is too much
            value_as_string = double_to_string(v, 1);
         }
         if (graph_type == "mll") {
            tw_offset = 10; // 24;
            v = 5.0 * f;
            y = 96.0 * f; // 100 is too much
            value_as_string = double_to_string(v, 0);
         }
         point_t p(x_offset - tw_offset, y + y_offset - 2);
         point_t pc = p.canvas_convert();
         std::string l = "   <text font-family=\"Helvetica, sans-serif\" font-size=\"6\" ";
         l += "x=\"";
         l += std::to_string(pc.x);
         l += "\" y=\"";
         l += std::to_string(pc.y);
         l += "\">";
         l += value_as_string;
         l += "</text>\n";
         s += l;
      }
      return s;
   }
 
   std::string make_main_title(const std::string &title_string) const {
      point_t p(12 + x_offset, 110 + y_offset);
      point_t pc = p.canvas_convert();
      std::string s =  "   <text font-family=\"Helvetica, sans-serif\" font-size=\"9\" ";
      s += "x=\"";
      s += std::to_string(pc.x);
      s += "\" y=\"";
      s += std::to_string(pc.y);
      s += "\"";
      s += ">";
      s += title_string;
      s += "</text>\n";
      return s;
   }

   std::string graph_internals(const std::string &graph_type) {

      point_t origin(x_offset, y_offset);
      point_t x_axis_max(x_offset + 100 + 10, y_offset);
      point_t y_axis_max(0 + x_offset, y_offset + 100 + 5);
      std::string line_x_axis = make_line(origin, x_axis_max, "#111111", 1, false, true);
      std::string line_y_axis = make_line(origin, y_axis_max, "#111111", 1, false, true);
      std::string parts = line_x_axis + line_y_axis;
      int n_cycles = 10;
      std::vector<std::string> grid_lines = make_grid_lines(n_cycles);
      for (const auto &line : grid_lines)
         parts += line;
      if (graph_type == "fsc_average") {
         std::vector<std::string> graph_lines = make_graph_lines("fsc_average");
         for (const auto &line : graph_lines)
            parts += line;
      }
      if (graph_type == "mll") {
         std::vector<std::string> graph_lines = make_graph_lines("mll");
         for (const auto &line : graph_lines)
            parts += line;
      }

      std::string x_axis_text = "Cycle Number";
      std::string y_axis_text;
      if (graph_type == "fsc_average") y_axis_text = "FSC";
      if (graph_type == "mll")         y_axis_text = "-LL/M";

      std::string x_axis_label = make_x_axis_label(x_axis_text);
      std::string y_axis_label = make_y_axis_label(y_axis_text);

      parts += x_axis_label;
      parts += y_axis_label;

      std::string tick_marks_x = make_tick_marks_x_axis(n_cycles);
      std::string tick_marks_y = make_tick_marks_y_axis();

      parts += tick_marks_x;
      parts += tick_marks_y;

      std::string tick_labels_x = make_x_axis_tick_labels(n_cycles, x_scale, x_offset);
      std::string tick_labels_y = make_y_axis_tick_labels(graph_type, x_scale, x_offset);

      parts += tick_labels_x;
      parts += tick_labels_y;

      std::string graph_title;
      if (graph_type == "fsc_average") graph_title = "Summary Data FSC";
      if (graph_type == "mll")         graph_title = "Summary Data -LL";

      std::string title = make_main_title(graph_title);

      parts += title;

      return parts;
      
   }
};

class binned_data_t {
public:
   binned_data_t(const double &reso_d, const double &fsc_FC_full, const double &Rcmplx_FC_full,
                 const double &cc_FC_full, const double &mcos_FC_full) : reso(reso_d), fsc_FC_full(fsc_FC_full),
                                                                         Rcmplx_FC_full(Rcmplx_FC_full), cc_FC_full(cc_FC_full),
                                                                         mcos_FC_full(mcos_FC_full) {}
   double reso;
   double fsc_FC_full;
   double Rcmplx_FC_full;
   double cc_FC_full;
   double mcos_FC_full;

};

class binned_data_container_t {
public:
   std::map<int, std::vector<binned_data_t> > data;
   void add(int cycle, const binned_data_t &b) { data[cycle].push_back(b); }
   void output() {

      std::string output_file_name = "binned-data.svg";
      std::string s = make_svg();
      std::ofstream f(output_file_name);
      f << s;
      f.close();
   }

   std::string make_svg() const {
      std::string s;
      return s;
   }
};

void
parse_cycle(json j, summary_data_container_t *summary_data_container_p, binned_data_container_t *bdc_p) {

   int nth_cycle = j["Ncyc"];
   json j_data = j["data"];
   json j_summary = j_data["summary"];
   float fsc_average = j_summary["FSCaverage"];
   float mll = j_summary["-LL"];
   std::cout << nth_cycle << " " << fsc_average << " " << mll << std::endl;
   summary_data_container_p->add(summary_data_t(nth_cycle, fsc_average, mll));

   // "binned" data inside "data"
   json j_binned = j_data["binned"];
   unsigned int s = j_binned.size();
   std::cout << "   " << s << std::endl;
   for (unsigned int i=0; i<s; i++) {
      json binned_data = j_binned[i];
      double d_min          = binned_data["d_min"];
      double d_max          = binned_data["d_max"];
      double fsc_FC_full    = binned_data["fsc_FC_full"];
      double Rcmplx_FC_full = binned_data["Rcmplx_FC_full"];
      double cc_FC_full     = binned_data["cc_FC_full"];
      double mcos_FC_full   = binned_data["mcos_FC_full"];
      double d_mid = 2.0/(1.0/d_min + 1.0/d_max);
      binned_data_t bd(d_mid, fsc_FC_full, Rcmplx_FC_full, cc_FC_full, mcos_FC_full);
      bdc_p->add(nth_cycle, bd);
   }
}

int main(int argc, char **argv) {

   auto file_to_string = [] (const std::string &file_name) {
      std::fstream f(file_name);
      std::string s;
      f.seekg(0, std::ios::end);
      s.reserve(f.tellg());
      f.seekg(0, std::ios::beg);
      s.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
      return s;
   };

   int status = 0;
   if (argc > 1) {

      std::string json_file_name = argv[1];
      try {

         summary_data_container_t summary_data_container;
         binned_data_container_t binned_data_container;
         std::string s = file_to_string(json_file_name);
         json j = json::parse(s);
         int cycle_index = 0;
         while (true) {

            try {
               json item = j.at(cycle_index);
               parse_cycle(item, &summary_data_container, &binned_data_container);

               // next time
               cycle_index++;
            }
            catch (const nlohmann::detail::out_of_range &oor) {
               // std::cout << "out of range " << cycle_index << std::endl;
               break;
            }
         }

         summary_data_container.output();
      }
      catch (const std::runtime_error &e) {
         std::cout << "WARNING::" << e.what() << std::endl;
         status = 2; // file not found
      }
   } else {
      std::cout << "Usage: servalcat-tracker json-file-name" << std::endl;
   }

   return status;
}
