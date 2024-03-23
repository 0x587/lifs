package mem

import (
	"context"
	"lifs_go/cas"
	"lifs_go/cas/chunks"
	"lifs_go/cas/store"
)

type Key struct {
	Key   cas.Key
	Type  string
	Level uint8
}

type Mem struct {
	data map[Key][]byte
}

var _ store.Store = (*Mem)(nil)

func (m *Mem) get(ctx context.Context, key cas.Key, type_ string, level uint8) ([]byte, error) {
	data := m.data[Key{key, type_, level}]
	return data, nil
}

func (m *Mem) Get(ctx context.Context, key cas.Key, type_ string, level uint8) (*chunks.Chunk, error) {
	return store.HandleGet(ctx, m.get, key, type_, level)
}

func (m *Mem) Add(ctx context.Context, c *chunks.Chunk) (key cas.Key, err error) {
	key = chunks.Hash(c)
	if m.data == nil {
		m.data = make(map[Key][]byte)
	}
	m.data[Key{key, c.Type, c.Level}] = c.Buf
	return key, nil
}
