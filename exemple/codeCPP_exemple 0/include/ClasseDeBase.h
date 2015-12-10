#ifndef CLASSEDEBASE__H
#define CLASSEDEBASE__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include "ClasseC.h"
#include "ClasseA.h"
#include <memory>
#include "ClasseB.h"





/////////////////////////////////////////////////
/// \brief Classe qui h�rite de 2 autres classes. Cela ajoutera les includes nec�ssaires.
///
/////////////////////////////////////////////////
class ClasseDeBase : public ClasseC, public ClasseA {



/////////////////////////////////////////////////
// M�thodes
/////////////////////////////////////////////////
public:
    /////////////////////////////////////////////////
    /// \brief Constructeur, les attributs ayant une valeur par d�faut vont �tre initialis�s dans le *.cpp.
    ///
    /////////////////////////////////////////////////
    ClasseDeBase ();

    /////////////////////////////////////////////////
    /// \brief operation public de base.
    ///
    /// \param param1		 Parametre de l'operation.
    /////////////////////////////////////////////////
    void operation_1 (int param1);

    /////////////////////////////////////////////////
    /// \brief operation virtuelle qui override celle de la ClasseA.
    ///
    /////////////////////////////////////////////////
    virtual void operation_virtuelle ();

    /////////////////////////////////////////////////
    /// \brief operation virtuelle qui override l'operation abstraite de la ClasseA.
    ///
    /////////////////////////////////////////////////
    virtual void operation_abstraite ();



/////////////////////////////////////////////////
// Membres
/////////////////////////////////////////////////
public:
    static int ms_attr_1;    ///< Attribut static de la classe de base. Sera initialis� au d�but du *.cpp.    
    int m_attr_2;    ///< Attribut public de la classe de base.    
protected:
    int m_attr_3;    ///< Attribut proteg� de la classe de base.    
    ClasseB m_B_composition;     ///< associ� avec le type: 'composition', cr�er un simple attribut.
    std::shared_ptr<ClasseB> m_B_aggregation;     ///< associ� avec le type: 'Aggregation'. Cr�er l'attribut dans un shared_ptr.

}; // fin class ClasseDeBase

#endif
