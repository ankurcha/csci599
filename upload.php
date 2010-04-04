<?php
    
    $file = $_POST['F1'];
    $target_path = "uploads/";

    $target_path = $target_path . basename( $HTTP_POST_FILES['F1']['name']); 

    if(move_uploaded_file($HTTP_POST_FILES['F1']['tmp_name'], $target_path)) {
        echo "The file ".  basename( $HTTP_POST_FILES['F1']['name']). " has been uploaded";
    } else{
            echo "There was an error uploading the file, please try again!";
    }
?>
