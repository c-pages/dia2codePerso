#ifndef GEOMETRIE__H
#define GEOMETRIE__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>



namespace gui {

class Skin;
class Gadget;


/////////////////////////////////////////////////
/// \brief Concerne la partie geometrique des gadgets, comme la taille, collision...
///
/////////////////////////////////////////////////
class Geometrie {


/////////////////////////////////////////////////
// Enums & typedefs
/////////////////////////////////////////////////
    typedef std::shared_ptr<Gadget> ptr;    ///< Pointeur vers un gadget.


/////////////////////////////////////////////////
// Méthodes
/////////////////////////////////////////////////

public:
public:
    /////////////////////////////////////////////////
    /// \brief Acceder à la position absolue du gadget.
    ///
    /////////////////////////////////////////////////
    sf::Vector2f getPosAbs () const;

    /////////////////////////////////////////////////
    /// \brief Définir la taille du gadget (m_taille, qui sert de cadre pour dessiner le gadget.)
    ///
    /// \param taille		 
    /////////////////////////////////////////////////
    void setSize (sf::Vector2i taille);

    /////////////////////////////////////////////////
    /// \brief Acceder à la taille du gadget (m_taille).
    ///
    /////////////////////////////////////////////////
    sf::Vector2i getSize () const;

    /////////////////////////////////////////////////
    /// \brief Acceder à la BB du gadget en coordonnées local (m_localBounds)..
    ///
    /////////////////////////////////////////////////
    sf::FloatRect getLocalBounds () const;

    /////////////////////////////////////////////////
    /// \brief  Acceder à la BB du gadget en coordonnées global (m_globalBounds).
    ///
    /////////////////////////////////////////////////
    sf::FloatRect getGlobalBounds () const;

    /////////////////////////////////////////////////
    /// \brief Définir le skin courant du gadget.
    ///
    /// \param skin		 le nouveau skin
    /////////////////////////////////////////////////
    void setSkin (std::shared_ptr<Skin> skin);

    /////////////////////////////////////////////////
    /// \brief S'aligner sur un autre gadget.
    ///
    /// \param cible		 gadget sur lequel s'aligner.
    /////////////////////////////////////////////////
    void Aligner (ptr cible);

    /////////////////////////////////////////////////
    /// \brief  Test pour savoir si le point (x,y) survol le gadget.
    ///
    /// \param x		 
    /// \param y		 
    /////////////////////////////////////////////////
    bool testerSurvol (float x, float y) const;

    /////////////////////////////////////////////////
    /// \brief Actualiser les membres bounds local et global.
    ///
    /////////////////////////////////////////////////
    virtual void actualiser_bounds ();



/////////////////////////////////////////////////
// Membres
/////////////////////////////////////////////////
private:
    sf::Vector2i m_taille;    ///< la taille permet d'organiser la disposition des éléments graphiques du gadget.    
    sf::FloatRect m_localBounds;    ///< la rectangle anglobant du gadget en coordonnés locales.    
    sf::FloatRect m_globalBounds;    ///< la rectangle anglobant du gadget en coordonnés globales.    
    std::shared_ptr<Skin> m_skin;    ///< pointeur vers le skin du gadget    
protected:
    std::vector<ptr> m_composants;    ///< les differents gadgets qui composent ce gadget.    

}; // fin class Geometrie

} // fin namespace gui

#endif
