#ifndef CLASSEA__H
#define CLASSEA__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include <memory>
#include <vector>



namespace popo {

class ClasseB;


/////////////////////////////////////////////////
/// \brief Dans la ClasseA on colle une d�finition de type et on utilise ce type dans 2 attributs associ�s en multiple (dans des vectors (*) 
///
/////////////////////////////////////////////////
class ClasseA {


/////////////////////////////////////////////////
// Enums & typedefs
/////////////////////////////////////////////////
    typedef std::shared_ptr<ClasseB> ptr;    ///< Celui ci est integr� comme definition de type car la liaison est vide.


/////////////////////////////////////////////////
// M�thodes
/////////////////////////////////////////////////

public:
    ///< Ajouter un �lement dans m_compositionMultiple
    void ajouterACompositionMultiple ( ptr nouvelElement ){ m_compositionMultiple.push_back( nouvelElement ); };

    ///< retirer l'�lement � la position id dans m_compositionMultiple
    void retirerACompositionMultiple ( int id ) { if ( id>=0 || id<m_compositionMultiple.size() ) m_compositionMultiple.erase( m_compositionMultiple.begin() + id ); };

    ///< Vider m_compositionMultiple
    void viderCompositionMultiple () { m_compositionMultiple.clear(); };

    ///< Accesseur � l'�l�ment de m_compositionMultiple d�sign� par un id.
    ptr getCompositionMultiple ( int id ) const { if ( id>=0 || id<m_compositionMultiple.size() )  return m_compositionMultiple.at( id ); else return 0; };

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
    /// \brief operation abstraite : dans Dia: Abstraite. Doit etre overrid� par h�riti�. N'est par d�finie dans le *.cpp.
    ///
    /////////////////////////////////////////////////
    virtual void operation_abstraite () = 0;



/////////////////////////////////////////////////
// Membres
/////////////////////////////////////////////////
public:
    float m_attr_A1;    ///< Attribut direct dans les propri�t�s de ClasseA.    
    std::vector<ptr>  m_compositionMultiple;     ///< multiplicit�:  * ou 0..* : creation vector de cet attribut.  En mettant #A, #R, #V, #G dans le commentaire d'un vector d'attributs on indique a dia2code de prevoir les fonctions ajouter, retirer, vider, get dans le *.h

}; // fin class ClasseA

} // fin namespace popo

#endif
