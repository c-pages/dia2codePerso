#ifndef CLASSEA__H
#define CLASSEA__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include <memory>
#include <vector>



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
    /// \brief operation abstraite : dans Dia: Abstraite. Doit etre overrid� par h�riti�.
    ///
    /////////////////////////////////////////////////
    virtual void operation_abstraite () = 0;



/////////////////////////////////////////////////
// Membres
/////////////////////////////////////////////////
public:
    float m_attr_A1;    ///< Attribut direct dans les propri�t�s de ClasseA.    
    std::vector<std::shared_ptr<ptr>>  m_B_agregationMultiple;     ///< multiplicit�:  * ou 0..* : creation vector de cet attribut.
    std::vector<ptr>  m_B_compositionMultiple;     ///< multiplicit�:  * ou 0..* : creation vector de cet attribut.

}; // fin class ClasseA

#endif
