# dia2codePerso
customisation à la cerpe de dia2code http://dia2code.sourceforge.net/
  - commentaires (facon SFML)  (Visible C++ Style de code::block).
  - creation des*.cpp.
  - gestion des virtuel, abstrait et statics, avec initialisation des statics dans le *.cpp.
  - gestion des includes.
  - creation automatique des accesseurs par ajout de tag dans le commentaire de l'attribut:
	- pour attributs simple : #G:get, #S:set
	- pour vector d'attributs : #A:Ajouter, #R:Retirer, #V:vider, #G:getIndex





INSTALLE windaube à l'arrache mais ca marche

	
1> 	copier le contenu du dossier "bin/Release" dans "C:\Program Files (x86)\dia2code"

2>	ajouter un chemin dans la liste des paths
			("My Computer" > "Properties" > "Advanced" > "Environment Variables" > "Path")
			vers le dossier "C:\Program Files (x86)\dia2code"
			

3>	copier un raccourci vers "C:\Program Files (x86)\dia2code\dia2codeBAT.bat" dans le dossier sendto (shell:sendto)


4>	=> clique droit sur un fichier *.dia => "envoyer vers"  => "dia2code"
