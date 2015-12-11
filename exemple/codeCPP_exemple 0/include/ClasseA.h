#ifndef CLASSEA__H
#define CLASSEA__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include <memory>
#include <vector>



class ClasseB;


/////////////////////////////////////////////////
/// \brief Dans la ClasseA on colle une définition de type et on utilise ce type dans 2 attributs associés en multiple (dans des vectors (*) 
///
/////////////////////////////////////////////////
class ClasseA {


/////////////////////////////////////////////////
// Enums & typedefs
/////////////////////////////////////////////////
    typedef std::shared_ptr<ClasseB> ptr;    ///< Celui ci est integré comme definition de type car la liaison est vide.


/////////////////////////////////////////////////
// Méthodes
/////////////////////////////////////////////////

public:
    ///< Ajouter un élement dans m_compositionMultiple
    void ajouterCompositionMultiple ( std::shared_ptr<ClasseB> nouvelElement ){ m_compositionMultiple.push_back( nouvelElement ); };

    ///< retirer l'élement à la position id dans m_compositionMultiple
    void retirerCompositionMultiple ( int id ) { if ( id>=0 || id<m_compositionMultiple.size() ) m_compositionMultiple.erase( m_compositionMultiple.begin() + id ); };

    ///< Vider m_compositionMultiple
    void viderCompositionMultiple () { m_compositionMultiple.clear(); };

    ///< Accesseur à l'élément de m_compositionMultiple désigné par un id.
    std::shared_ptr<ClasseB> getCompositionMultiple ( int id ) const { if ( id>=0 || id<m_compositionMultiple.size() )  return m_compositionMultiple.at( id ); else return 0; };

public:
    /////////////////////////////////////////////////
    /// \brief operation simple
    ///
    /////////////////////////////////////////////////
    bool operation_A1 ();

    /////////////////////////////////////////////////
    /// \brief operation virtuelle : dans Dia: polymorphe(virtuelle).
    ///
    /////////////////////////////////////////////////
    virtual void operation_virtuelle ();

    /////////////////////////////////////////////////
    /// \brief operation abstraite : dans Dia: Abstraite. Doit etre overridé par héritié. N'est par définie dans le *.cpp.
    ///
    /////////////////////////////////////////////////
    virtual void operation_abstraite () = 0;



/////////////////////////////////////////////////
// Membres
/////////////////////////////////////////////////
public:
    float m_attr_A1;    ///< Attribut direct dans les propriétés de ClasseA.    
    std::vector<ptr>  m_compositionMultiple;     ///< multiplicité:  * ou 0..* : creation vector de cet attribut.  En mettant #A, #R, #V, #G dans le commentaire d'un vector d'attributs on indique a dia2code de prevoir les fonctions ajouter, retirer, vider, get dans le *.h

}; // fin class ClasseA

#endif
