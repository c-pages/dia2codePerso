#ifndef GUI__H
#define GUI__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include "Groupe.h"
#include "Drawable.h"


namespace gui {

    /////////////////////////////////////////////////
    /// \brief createur de gadget
    ///
    /////////////////////////////////////////////////
    class Creer {
        
        /////////////////////////////////////////////////
        // Les methodes
        /////////////////////////////////////////////////
        
    public:
        ptr label ();

        ptr image ();

        ptr bouton ();

        ptr fenetre ();

    };

    /////////////////////////////////////////////////
    /// \brief Le gestionnaire (unique) des gadgets
    ///
    /////////////////////////////////////////////////
    class GUI : public Drawable {
        
        /////////////////////////////////////////////////
        // Les methodes
        /////////////////////////////////////////////////
        
    public:
        void traiterEvenements (const sf::Event&  evenement);

        void draw (sf::RenderTarget& target, sf::RenderStates states) const;

        
        /////////////////////////////////////////////////
        // Les membres
        /////////////////////////////////////////////////
        
    public:
        Creer creer;        
         m_gestRessources;        ///<  Le gestionnaire des resources propres aux interfaces.        
    private:
        vector<std::shared_ptr<gadgets>> m_gadgets;        ///<  la liste total des gadgets de l'interface        
        Groupe m_enfants;        
        sf::renderWindow m_fenetreSFML;        
        Skin m_skin;        
    };

};

#endif


////////////////////////////////////////////////////////////
/// class gui
/// ingroup 
///
/// see 
///
////////////////////////////////////////////////////////////
