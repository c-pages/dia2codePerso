#ifndef SKIN__H
#define SKIN__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include <memory>
#include "Style.h"



namespace gui {



/////////////////////////////////////////////////
/// \brief Un skin regroupe un ensemble de style pour composer tout les éléments d'une interface.
///
/////////////////////////////////////////////////
class Skin {



/////////////////////////////////////////////////
// Méthodes
/////////////////////////////////////////////////
public:
    /////////////////////////////////////////////////
    /// \brief Createur par défaut
    ///
    /////////////////////////////////////////////////
    Skin ();



/////////////////////////////////////////////////
// Membres
/////////////////////////////////////////////////
    std::shared_ptr<gui::Style> invisible;
    std::shared_ptr<gui::Style> cadre;
    std::shared_ptr<gui::Style> txtLog;
    std::shared_ptr<gui::Style> txtCourant;
    std::shared_ptr<gui::Style> txtTitre;
    std::shared_ptr<gui::Style> btnPress;
    std::shared_ptr<gui::Style> btnSurvol;
    std::shared_ptr<gui::Style> btnRepos;
    std::shared_ptr<gui::Style> fenetre;
    gui::Style desactive;

}; // fin class Skin

} // fin namespace gui

#endif
