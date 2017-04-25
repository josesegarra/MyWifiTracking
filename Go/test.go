

package main

import (
	"fmt"
	"io"
	"net/http"
  "flag"
	"time"
  "unsafe"
)


// #cgo CFLAGS:  -DCONFIG_LIBNL
// #cgo CFLAGS: -pthread
// #cgo LDFLAGS: -pthread
// #cgo CFLAGS: -I.
// #cgo LDFLAGS: -L. -lgb -losdep -lnl -lssl -lpthread
// #include <stdlib.h>
// #include <../C/c_main.h>
import "C"

// https://golang.org/cmd/cgo/
// https://www.digitalocean.com/community/tutorials/how-to-configure-a-linux-service-to-start-automatically-after-a-crash-or-reboot-part-1-practical-examples
// http://stackoverflow.com/questions/22336075/linux-process-into-a-service

func devices(w http.ResponseWriter){
	stream:=C.device_list()
  w.Header().Set("Content-Type","application/json; charset=utf-8")

	io.WriteString(w,"{\"time\":\""+time.Now().Format("2006-01-02 15:04:05")+"\",\n")
	io.WriteString(w,"\"devices\":")
	io.WriteString(w,C.GoString((*C.char)(stream.block)))
	io.WriteString(w,"}\n")

  C.stream_free(stream)
}

func main() {
  var iface string
  flag.StringVar(&iface, "iface", "mon0", "Interface to listen. Defaults to [mon0]")
  flag.Parse()
  fmt.Println("Listening on interface: ",iface)

  var ciface *C.char
  ciface=C.CString(iface)
  C.init(ciface)

	server:=CreateServer(":8000")
	server.AddRoute("/devices",devices)

	server.Listen()
  C.free(unsafe.Pointer(ciface))

}
