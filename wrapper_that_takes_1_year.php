<?php
/*
*/
$url = $_GET['url'];
$destination = $_GET['destination'];
$videoID = $_GET['videoID'];
$requestType = 'put';

passthru('/Users/ankurcha/Desktop/live/testProgs/openRTSP -4 -d 15 '.$url.' 1>/tmp/'.$videoID.'.mp4 &', $result);

if(!$result){
    echo 'Fail';
    $requestType = 'error';
}
/* send the file via curl to jason  */

passthru('curl -F "F1=@"/tmp/'.$vid.'.mp4 -F "videoID='.$videoID.'" -F "blockID=0" -F "requestType='.$requestType.'" '.$destination);
?>
