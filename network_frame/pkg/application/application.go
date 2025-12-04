package application

// Data 表示应用层数据
type Data struct {
	Payload []byte
}

// NewData 创建新的应用层数据
func NewData(payload string) *Data {
	return &Data{
		Payload: []byte(payload),
	}
}

// NewDataFromBytes 从字节切片创建应用层数据
func NewDataFromBytes(payload []byte) *Data {
	return &Data{
		Payload: payload,
	}
}

// GetPayload 获取原始数据
func (d *Data) GetPayload() []byte {
	return d.Payload
}

// GetPayloadString 获取原始数据字符串形式
func (d *Data) GetPayloadString() string {
	return string(d.Payload)
}

// Size 获取数据大小
func (d *Data) Size() int {
	return len(d.Payload)
}
