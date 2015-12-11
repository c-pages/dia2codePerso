# dia2codePerso
customisation à la cerpe de dia2code http://dia2code.sourceforge.net/
  - commentaires (facon SFML)  (Visible C++ Style de code::block).
  - creation des*.cpp avec initialisation des membres pour fonction de creation.
  - gestion des virtuel, abstrait et statics, avec initialisation des statics dans le *.cpp.
  - gestion des includes.
  - creation automatique des accesseurs avec ajout de tag dans le commentaire de l'attribut:
	- pour attributs simple : #G:get, #S:set
	- pour vector d'attributs : #A:Ajouter, #R:Retirer, #V:vider, #G:getIndex





INSTALLE perso à l'arrache mais ca marche

	
1>	ajouter un chemin dans la liste des paths
			("My Computer" > "Properties" > "Advanced" > "Environment Variables" > "Path")
			vers le dossier "....\dia2codePerso\bin\Release\"
			
2> 	copier le dossier "shell" dans "C:\Windows\"

3>	copier un raccourci vers "C:\Windows\shell\dia2codeBAT.bat" dans le dossier sendto (shell:sendto)



4>	enjoy:	clique droit sur un fichier *.dia => "envoyer vers"  => "dia2code" !
