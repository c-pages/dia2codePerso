#ifndef CLASSEB__H
#define CLASSEB__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include <vector>
#include <SFML/Graphics.hpp>
#include <memory>



namespace pipi {

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
    ///< Ajouter un élement dans m_machins
    void ajouterAMachins ( int nouvelElement ){ m_machins.push_back( nouvelElement ); };

    ///< retirer l'élement à la position id dans m_machins
    void retirerAMachins ( int id ) { if ( id>=0 || id<m_machins.size() ) m_machins.erase( m_machins.begin() + id ); };

    ///< Vider m_machins
    void viderMachins () { m_machins.clear(); };

    ///< Accesseur à l'élément de m_machins désigné par un id.
    int getMachins ( int id ) const { if ( id>=0 || id<m_machins.size() )  return m_machins.at( id ); else return 0; };

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
    std::vector<int> m_machins;    ///< #A#R#V#G    
    sf::FloatRect m_TrucSFML;    ///< Un peu tricky : le type comprte "sf::" on include donc 'SFML/Graphics.hpp'.    
    std::shared_ptr<ClasseC> m_classeC;    ///< ceci ajoute l'include 'memory', definie temporairement la ClassC dans le *.h, et include ClasseC dans le *.cpp.    
    float m_attr_B1;    ///< Attribut direct dans les propriétés de ClasseB. #G pour ajouter une fonction get et #S pour set.    

}; // fin class ClasseB

} // fin namespace pipi

#endif
