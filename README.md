1. Clone Contiki-NG repository and at least checked out the coresdk_cc13xx_cc26xx submodule.

$ git clone https://github.com/contiki-ng/contiki-ng.git
$ cd contiki-ng
$ git submodule update --init arch/cpu/simplelink-cc13xx-cc26xx/lib/coresdk_cc13xx_cc26xx

2.Download the necessary software:

   2.1 TI's Code Composer Studio (CCS) with support for CC13xx/CC26xx devices installed.
   2.2 The ARM GCC add-on in CCS. In CCS:
        Navigate to View → CCS App Center.
        Search for ARM GCC and click Select.
   2.3 If you are using Linux:
        sudo apt-get install build-essential
