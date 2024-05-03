/*
 * MoleculesToTriangles/CXXClasses/ColorScheme.h
 *
 * Copyright 2009 by Martin Noble, University of Oxford
 * Author: Martin Noble
 *
 * This file is part of Coot
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef SelectionMapColorScheme_h
#define SelectionMapColorScheme_h

#include <list>
#include <map>
#include <algorithm>
#include <memory>
#include "ColorRule.h"
#include "CompoundSelection.h"

#if defined _OPENMP
// #include <omp.h> 20221206-PE why is omp needed in the colour scheme header?
#else
#if __APPLE__
#include <dispatch/dispatch.h>
#endif
#endif

#include "mmdb2/mmdb_manager.h"

class ColorScheme {
private:
    //Modelled
    std::string name;
    std::list<std::shared_ptr<ColorRule> > rules;
    //Not modelled
    
#if __APPLE__ && !defined _OPENMP
    dispatch_queue_t selectionQueue;
#endif
 
public:
    ColorScheme()  {
        name = std::string("empty");
        rules.clear();
#if __APPLE__ && !defined _OPENMP
        selectionQueue = dispatch_queue_create( "com.martin.Aesop.colorSchemeSelection", NULL );
#endif
    };
    ~ColorScheme(){
    };
    ColorScheme(std::shared_ptr<ColorRule> colorRule){
	rules.clear();
	rules.push_back(colorRule);
    };
    void addRule(std::shared_ptr<ColorRule> _rule){
        if (_rule) {
            if (std::find(rules.begin(), rules.end(), _rule) == rules.end()){
                if (_rule->getRank()<0) _rule->setRank(rules.size());
                rules.push_back(_rule);
                rules.sort(ColorRule::compareRank);
            }
        } else {
            std::cout << "Error:: ColorScheme::addRule(): null rule" << std::endl;
        }
    };
    void removeRule(std::shared_ptr<ColorRule> _rule){
        rules.remove(_rule);
        rules.sort(ColorRule::compareRank);
    };
    std::list<std::shared_ptr<ColorRule> > &getColorRules() {
        return rules;
    };
    void setName (const std::string &_name) {
        name = _name;
    };
    const std::string &getName() const {
        return name;
    };
    
    virtual FCXXCoord colorForAtom(const mmdb::Atom* atom, std::map<std::shared_ptr<ColorRule>, int> &handles){
        auto rule = rules.begin();
        FCXXCoord result(1.,1.,1.,1.);
        while (rule != rules.end()){
            int iRule = handles[(*rule)];
            bool selected = const_cast<mmdb::Atom*>(atom)->isInSelection(iRule);
            // std::cout << "atom " << atom << " iRule " << iRule << " selected: " << selected << "\n";
            if (const_cast<mmdb::Atom*>(atom)->isInSelection(iRule)){
				result = (*rule)->colorForAtom(atom);
            }
            rule++;
        }
        return result;
    };
    
    std::map<std::shared_ptr<ColorRule>,int> prepareForMMDB(mmdb::Manager *_mmdb) {
       // std::cout << "...prepareForMMDB() start" << std::endl;
       std::map<std::shared_ptr<ColorRule>,int> handles;
       // std::cout << "...prepareForMMDB() done handles" << std::endl;
       rules.sort(ColorRule::compareRank);
       // std::cout << "...prepareForMMDB() done sort" << std::endl;
       auto rule = rules.begin();
       // std::cout << "...prepareForMMDB() found first rule of " << rules.size() << std::endl;
       while (rule != rules.end()){
          // std::cout << "...prepareForMMDB() iterating rules " << std::endl;
          auto compoundSelection = (*rule)->getCompoundSelection();
          // std::cout << "...prepareForMMDB() got compound selection " << std::endl;
          int handleOfRule = compoundSelection->handleInMMDB(_mmdb);
          // std::cout << "...prepareForMMDB() got mmdb rule handle " << std::endl;
          handles[*rule] = handleOfRule;
          // std::cout << "...prepareForMMDB() added rule to map " << std::endl;
          rule++;
          // std::cout << "...prepareForMMDB() next rule... " << std::endl;
       }
       return handles;
    };
    
    void freeSelectionHandles(mmdb::Manager *_mmdb, std::map<std::shared_ptr<ColorRule>,int> &handles){
        auto rule = rules.begin();
        while (rule != rules.end()){
            int handleOfRule = handles[*rule];
            if (handleOfRule > 0){            
                _mmdb->DeleteSelection(handleOfRule);
            }
            rule++;
        }
        handles.clear();
    };
    
    static std::shared_ptr<ColorScheme> colorByElementScheme();
    static std::shared_ptr<ColorScheme> colorBySecondaryScheme();
    static std::shared_ptr<ColorScheme> colorBFactorScheme();
    static std::shared_ptr<ColorScheme> colorRampChainsScheme();
    static std::shared_ptr<ColorScheme> colorChainsScheme();
    static std::shared_ptr<ColorScheme> colorChainsSchemeWithColourRules(const std::vector<std::pair<std::string, std::string> > &colour_rules);
    static std::shared_ptr<ColorScheme> colorSchemeForColorName(const std::string name);
    
};

#endif
