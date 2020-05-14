# Distributed Photo Sharing Platform
A distributed Photo Sharing Application implemented using QT in C++ 🖼 🔗 (2017).

The program is a combination of a `client-server` and `peer-to-peer` model where a centralized server caters to all users who are clients to collect and send a list of active users, while each user acts as both a server and client to other users.
This is a fully custom object oriented implementation from the front-end implemented using `QT`, down to the sockets themselves. `Steganography` is used to embed the number of views that the sender allocates when sending the image, this is read by the receiver side and when that number of views is finished, a default image is shown instead.

### Features:
* Select a photo to send another user from a specified folder and specify the amount of views allowed.
* Receive photos from other users and view them a specified amount of times.
* Request a list of active users.
* Request more views for a specific photo from its owner.
* Send more views for a photo you sent to another user.
* A default image is shown when the number of views is reached.

# Contact

* email: mfzs1@st-andrews.ac.uk
* LinkedIn: www.linkedin.com/in/malak-sadek-17aa65164/
* website: https://malaksadek.wordpress.com/
