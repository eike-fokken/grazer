#!/usr/bin/env sh

echo "copying mounted ssh folder into ~/.ssh"
# see https://stackoverflow.com/questions/27069193/how-can-i-mount-a-users-existing-ssh-directory-into-a-docker-container
# and https://nickjanetakis.com/blog/docker-tip-56-volume-mounting-ssh-keys-into-a-docker-container
sudo rm --recursive ~/.ssh
cp -R /tmp/.mounted_ssh ~/.ssh
sudo chmod 700 ~/.ssh
sudo chmod 644 ~/.ssh/id_rsa.pub
sudo chmod 600 ~/.ssh/id_rsa

echo "finished copying"
