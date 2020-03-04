package main

import (
	"bufio"
	"bytes"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"runtime"
	"strconv"
	"strings"
	"sync"
)

type node struct {
	letter byte
	row    int
	col    int
	edges  []*node
}

type rc struct {
	r int
	c int
}

type graph struct {
	word  string
	nodes map[rc]*node
}

type path []*node

func usage() {
	fmt.Println(`
usage: word_graph [OPTIONS] [FILE]

Solve the word graph (se accompying README) in
FILE (or read from stdin if no FILE or FILE is '-')

OPTIONS:`)

	flag.PrintDefaults()
}

func main() {
	list := flag.Bool("l", false, "list paths instead of count")
	word := flag.String("w", "", "word to search for (usually embedded in input file")
	u := flag.Bool("?", false, "help")

	flag.Parse()
	if *u {
		usage()
		return
	}

	var fh io.Reader
	switch flag.Arg(0) {
	case "", "-":
		fh = os.Stdin
	default:
		h, err := os.Open(flag.Arg(0))
		if err != nil {
			log.Fatal(err)
		}
		defer h.Close()
		fh = h
	}

	g := newGraph(fh, *word)
	g.findPaths(*list)
}

func newGraph(fh io.Reader, word string) graph {
	scanner := bufio.NewScanner(fh)
	var lines []string

	g := graph{word: word, nodes: make(map[rc]*node)}

	for scanner.Scan() {
		l := scanner.Text()
		t := strings.TrimSpace(l)

		// ignore empty lines and comments
		if len(t) == 0 || t[0] == '#' {
			continue
		}

		if a := strings.SplitN(t, "=", 2); len(a) > 1 {
			g.processAssignment(a)
			continue
		}

		lines = append(lines, l)
	}

	if g.word == "" {
		log.Fatal("No word defined")
	}

	if len(lines) > len(g.word)*2-1 {
		log.Fatalf("Too many lines in input graph for word '%v'", g.word)
	}

	for r := 0; r < len(lines); r += 2 {
		g.parseNodeLine(r, lines[r])
		if r > 0 {
			g.parseEdgeLine(r-1, lines[r-1])
		}
	}

	return g
}

func (g *graph) findPaths(printPaths bool) {
	found := 0

	// Create a 'bucket' of paths we can reuse instead of constantly
	// allocating (and freeing) new ones
	chPathBucket := make(chan path, runtime.GOMAXPROCS(0))
	for i := 0; i < runtime.GOMAXPROCS(0); i++ {
		chPathBucket <- path(make([]*node, len(g.word)))
	}

	// Channel for paths to be printed when printPaths is true
	pch := make(chan path)

	// Channel to write the count of found paths from each worker goroutine
	nch := make(chan int)

	// Spawn worker goroutines to search for paths in the graph
	// One worker for each node that is a starting letter
	var wg sync.WaitGroup
	for _, n := range g.nodes {
		if n.letter != g.word[0] {
			continue
		}

		wg.Add(1)
		go func(n *node) {
			p := path(make([]*node, len(g.word)))
			p[0] = n
			var nr int
			if printPaths {
				nr = g.search(pch, chPathBucket, n, p, 1)
			} else {
				nr = g.search(nil, chPathBucket, n, p, 1)
			}
			nch <- nr
			wg.Done()
		}(n)
	}

	go func() {
		wg.Wait()
		// workers are done, close the pth channel to signal search completion
		close(pch)
	}()

	// Spawn workers to receive and print found paths
	go func() {
		var wg sync.WaitGroup
		out := make(chan string)

		for i := 0; i < runtime.GOMAXPROCS(0); i++ {
			wg.Add(1)
			// receive paths, generate the string to output and pass it on
			go func() {
				for p := range pch {
					out <- p.String()
					chPathBucket <- p
				}
				wg.Done()
			}()
		}

		go func() {
			wg.Wait()
			// workers are done, so searching and listing is complete
			// close our out channel to signal this completion
			close(out)
		}()

		// receive and print path strings
		for s := range out {
			fmt.Println(s)
		}

		// And finally we're done outputting paths, so we can close the nch
		// channel to signal the main routine to exit
		close(nch)
	}()

	// Receive path counts and accumulate a total
	for n := range nch {
		found += n
	}

	if !printPaths {
		fmt.Printf("Found %v paths that spell '%s'\n", found, g.word)
	}
}

func (g *graph) search(ch, chPathBucket chan path, n *node, p path, ofs int) int {
	nr := 0

	for _, e := range n.edges {
		if e.letter != g.word[ofs] {
			continue
		}

		p[ofs] = e

		if ofs+1 == len(g.word) {
			// we found a path
			if ch != nil {
				pc := <-chPathBucket
				copy(pc, p)
				ch <- pc
			}
			nr++
			continue
		}

		nr += g.search(ch, chPathBucket, e, p, ofs+1)
	}

	return nr
}

func (g *graph) processAssignment(a []string) {
	k := strings.TrimSpace(a[0])
	v := strings.TrimSpace(a[1])

	switch k {
	case "word":
		if g.word == "" {
			g.word = v
		}
	}
}

func (g *graph) parseNodeLine(r int, t string) {
	if len(t) > len(g.word)*2-1 {
		log.Fatalf("[%v]: Line too long for word '%v'", r, g.word)
	}

	valid := g.word + " "
	for c := 0; c < len(t); c += 2 {
		if strings.IndexByte(valid, t[c]) == -1 {
			log.Fatalf("[%v][%v]: Invalid letter in input graph '%c'", r, c, t[c])
		}

		n := &node{letter: t[c], row: r, col: c}

		if c > 0 && t[c-1] == '-' {
			p, ok := g.nodes[rc{r, c - 2}]
			if !ok {
				log.Fatalf("[%v][%v]: Invalid edge '%c'", r, c-1, t[c-1])
			}
			p.edges = append(p.edges, n)
			n.edges = append(n.edges, p)

			//fmt.Printf("nl: %v-%v\n", n, p)
		}

		//fmt.Printf("Adding node at %v\n", rc{r, c})
		g.nodes[rc{r, c}] = n
	}
}

func (g graph) parseEdgeLine(r int, t string) {
	for c := 0; c < len(t); c++ {
		var n1, n2 *node

		switch t[c] {
		case '\\':
			n1 = g.nodes[rc{r - 1, c - 1}]
			n2 = g.nodes[rc{r + 1, c + 1}]
		case '/':
			n1 = g.nodes[rc{r - 1, c + 1}]
			n2 = g.nodes[rc{r + 1, c - 1}]
		case '|':
			n1 = g.nodes[rc{r - 1, c}]
			n2 = g.nodes[rc{r + 1, c}]
		}

		if n1 != nil && n2 != nil {
			//fmt.Printf("el: %v-%v\n", n1, n2)
			n1.edges = append(n1.edges, n2)
			n2.edges = append(n2.edges, n1)
		}
	}
}

func (g graph) dump() {
	fmt.Printf("word=%v\n", g.word)

	lines := make([][]byte, len(g.word)*2-1)
	for r := 0; r < len(g.word)*2-1; r++ {
		lines[r] = bytes.Repeat([]byte(" "), len(g.word)*2-1)
	}

	for _, n := range g.nodes {
		lines[n.row][n.col] = n.letter
		//fmt.Printf("--- %v\n", n)

		for _, e := range n.edges {
			if e.row < n.row {
				continue
			}

			var c byte = ' '
			row := (n.row + e.row) / 2
			col := (n.col + e.col) / 2

			switch {
			case e.row == n.row:
				c = '-'
			case e.col == n.col:
				c = '|'
			case e.col > n.col:
				c = '\\'
			case e.col < n.col:
				c = '/'
			}

			//fmt.Printf(": %v - %v : r=%v c=%v c=%c\n", n, e, row, col, c)
			lines[row][col] = c
		}
	}

	for _, l := range lines {
		fmt.Printf("%s\n", l)
	}
}

// strings.Builder is *much* faster than using fmt.Sprintf()
func (n *node) String() string {
	var s strings.Builder
	s.WriteByte(n.letter)
	s.WriteByte('[')
	s.WriteString(strconv.Itoa(n.row))
	s.WriteByte(',')
	s.WriteString(strconv.Itoa(n.col))
	s.WriteByte(']')
	return s.String()
}

func (p path) String() string {
	var s strings.Builder

	for i := 0; i < len(p); i++ {
		if i > 0 {
			s.WriteString("-")
		}
		if p[i] != nil {
			s.WriteString(p[i].String())
		}
	}
	return s.String()
}
