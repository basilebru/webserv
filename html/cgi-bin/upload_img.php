#!/usr/bin/php
<?php
$uploaddir = '../uploads/';
$uploadfile = $uploaddir . basename($_FILES['file']['name']);
echo("Content-Type: text/plain\n");
echo("Connection: keep-alive\n\n");

echo($uploadfile);

// echo '<pre>';
// if (move_uploaded_file($_FILES['userfile']['tmp_name'], $uploadfile)) {
//     echo "Le fichier est valide, et a été téléchargé
//            avec succès. Voici plus d'informations :\n";
// } else {
//     echo "Attaque potentielle par téléchargement de fichiers.
//           Voici plus d'informations :\n";
// }

// echo 'Voici quelques informations de débogage :';
// // print_r($_FILES);

// echo '</pre>';
?>