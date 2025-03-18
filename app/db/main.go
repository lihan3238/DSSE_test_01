package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"os"
	"sort"
)

// readJSON 读取 JSON 文件并解析为 map
func readJSON(filename string) (map[string][]int, error) {
	file, err := ioutil.ReadFile(filename)
	if err != nil {
		return nil, err
	}

	var data map[string][]int
	err = json.Unmarshal(file, &data)
	if err != nil {
		return nil, err
	}

	return data, nil
}

// intersection 计算多个集合的交集
func intersection(sets [][]int) []int {
	if len(sets) == 0 {
		return []int{}
	}

	result := sets[0]
	for _, set := range sets[1:] {
		result = intersectTwo(result, set)
	}
	return result
}

// intersectTwo 计算两个集合的交集
func intersectTwo(a, b []int) []int {
	m := make(map[int]bool)
	for _, v := range a {
		m[v] = true
	}

	var result []int
	for _, v := range b {
		if m[v] {
			result = append(result, v)
		}
	}
	return result
}

// union 计算多个集合的并集
func union(sets [][]int) []int {
	m := make(map[int]bool)
	for _, set := range sets {
		for _, v := range set {
			m[v] = true
		}
	}

	var result []int
	for k := range m {
		result = append(result, k)
	}

	sort.Ints(result)
	return result
}

func main() {
	if len(os.Args) < 3 {
		fmt.Println("Usage: go run main.go <filename> <keyword1> <keyword2> ...")
		return
	}

	filename := os.Args[1]
	keywords := os.Args[2:]

	data, err := readJSON(filename)
	if err != nil {
		fmt.Println("Error reading JSON:", err)
		return
	}

	var sets [][]int
	for _, key := range keywords {
		if values, found := data[key]; found {
			sets = append(sets, values)
		} else {
			fmt.Printf("Warning: key '%s' not found in JSON\n", key)
		}
	}

	if len(sets) == 0 {
		fmt.Println("No valid sets found.")
		return
	}

	inter := intersection(sets)
	uni := union(sets)

	fmt.Println("Intersection:", inter)
	fmt.Println("Union:", uni)
}
