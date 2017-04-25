package main

import (
  "fmt"
	"os"
	"path/filepath"
	"io"
	"io/ioutil"
	"net/http"
	"net/url"
	"mime"
)

type myServer struct{
    mux map[string]func(http.ResponseWriter)
		server http.Server
		root string
}

func  CreateServer(address string) (this *myServer){

		this =&myServer{}
		this.root,_ = filepath.Abs(filepath.Dir(os.Args[0]))
		this.root=this.root+"/www/"
		fmt.Println("Root folder: "+this.root)

		this.mux =make(map[string]func(http.ResponseWriter))
		this.server = http.Server{
	    Addr:    address,
			Handler: this }
		return
}

func (this *myServer) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	u, err := url.Parse(r.URL.String())
  if err != nil{
		return
	}
  if h, ok := this.mux[u.Path]; ok {
		h(w)
		return
	}
	filename:=this.root+u.Path
	extension :=filepath.Ext(filename)
	mime :=mime.TypeByExtension(extension)
	if (mime==""){
		mime="text/plain"
	}
	data, err := ioutil.ReadFile(filename)
	if err != nil {
		w.WriteHeader(http.StatusNotFound )
		io.WriteString(w, "Could not find "+r.URL.String()+"   Path: "+u.Path)
		return
	}
	w.WriteHeader(http.StatusOK)
	w.Header().Set("Content-Type", mime)
	w.Header().Set("Content-Length", fmt.Sprint(len(data)))
	fmt.Fprint(w, string(data))

}

func (this *myServer) AddRoute(route string, handler func(http.ResponseWriter)){
	this.mux[route]=handler
}

func (this *myServer) Listen(){
	this.server.ListenAndServe()
}
