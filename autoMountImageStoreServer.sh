#NAME : autoMountImageStoreServer.sh
#DESC : Connect to the remote server to gain the diskspace in the raspberry Pi sotrage. 
#VERS : KwangEun An - 2016.09.28 - mount remote directory and grant all priviliages in the mounted directory to user 'pi'.

#davfs requires to mount. Install 'davfs2' package. sudo apt-get install davfs2
$diretory = "StoreImage" #This directory must register in .gitignore to avoid uploading images.
if [ ! -d "$directory"]; then
  mkdir ./$directory
fi
sudo mount -t davfs http://203.250.32.155:5005/homes/supernova/www/SmartPlanting/StoredImages ./StoreImages #In this version, the private server provided to test captureImages.sh. The server may close without any warnning.
sudo chown -R pi:pi ./$directory
