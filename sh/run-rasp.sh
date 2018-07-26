qemu-system-arm -M versatilepb -cpu arm1176 -m 192 -hda $1 -kernel kernel-qemu -append "root=/dev/sda2" -net nic -net user,hostfwd=tcp::2222-:22,hostfwd=tcp::5800-:5800,hostfwd=tcp::5900-:5900  
