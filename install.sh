#!/bin/bash

set -e
git clone https://github.com/FinityFly/clitris.git
cd clitris
sudo mv clitris "/usr/local/bin/clitris"
echo "Installation complete. You can now run the game using 'clitris'."