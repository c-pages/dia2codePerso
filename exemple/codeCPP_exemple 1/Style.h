#ifndef STYLE__H
#define STYLE__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include <SFML/Graphics.hpp>



namespace gui {

class Style;


/////////////////////////////////////////////////
/// \brief Un style regroupe un ensemble de caractéristiques de base des gadgets.
///
/////////////////////////////////////////////////
class Style {



/////////////////////////////////////////////////
// Membres
/////////////////////////////////////////////////
public:
    sf::Font txt_police;    ///< Police du texte.    
    sf::Text::Style txt_style;    ///< Style SFML du texte (Regular, Italic, ... ).    
    float txt_taille;    ///< Taille du texte.    
    sf::Color txt_couleur;    ///< Couleur du texte.    
    float lgn_epaisseur;    ///< Epaisseur des lignes.    
    sf::Color lgn_couleur;    ///< Couleur des lignes.    
    sf::Color fnd_couleur;    ///< Couleur du fond.    
    sf::Texture fnd_texture;    ///< Texture du fond au repos.    

}; // fin class Style

} // fin namespace gui

#endif
