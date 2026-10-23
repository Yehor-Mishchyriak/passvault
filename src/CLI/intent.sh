# retrieval into clipboard
passvault --vault path/to/vault.pv -k $KEY --get -n wesportal |> cpb
# retrieval into terminal
passvault --vault path/to/vault.pv -k $KEY --get -n wesportal
# export into a global variable NAME
passvault --vault path/to/vault.pv -k $KEY --get -n wesportal -o NAME

# insertion
passvault --vault path/to/vault.pv -k $KEY --add \
    -n wesportal    \
    -d "wesportal password"     \
    -v ******************** \

# deletion
passvault --vault path/to/vault.pv -k $KEY --del -n wesportal
