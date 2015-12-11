#ifndef CLASSEB__H
#define CLASSEB__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <memory>
#include "ClasseC.h"



class ClasseC;


/////////////////////////////////////////////////
/// \brief associé avec le type: 'Aggregation'. Créer l'attribut dans un shared_ptr. En mettant #G ou #S dans le commentaire d'un attribut on indique a dia2code  de prevoir les fonctions get ou set dans le *.h.
///
/////////////////////////////////////////////////
class ClasseB {



/////////////////////////////////////////////////
// Méthodes
/////////////////////////////////////////////////

public:
    ///< Definir m_attr_B1
    void setAttr_B1( float val ){ m_attr_B1 = val; };

    ///< Acceder à m_attr_B1
    float getAttr_B1 () const { return m_attr_B1; };

public:
    /////////////////////////////////////////////////
    /// \brief operation simple
    ///
    /////////////////////////////////////////////////
    bool operation_B1 ();



/////////////////////////////////////////////////
// Membres
/////////////////////////////////////////////////
public:
    sf::FloatRect m_TrucSFML;    ///< Un peu tricky : le type comprte "sf::" on include donc 'SFML/Graphics.hpp'.    
    std::shared_ptr<ClasseC> m_classeC;    ///< ceci ajoute l'include 'memory', definie temporairement la ClassC dans le *.h, et include ClasseC dans le *.cpp.    
    float m_attr_B1;    ///< Attribut direct dans les propriétés de ClasseB. #G pour ajouter une fonction get et #S pour set.    
    std::vector<ClasseC>  m_lesClassesC;     ///< association multiple pour vector d'attributs.

}; // fin class ClasseB

#endif
