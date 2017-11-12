void usart_init(void); 
void usart_send_32(uint32_t , uint32_t * , uint8_t );
void process_command(char *cmd);
void usart_send_data(uint32_t USART, uint32_t *data, uint8_t lenth);
void usart_send_byte(uint32_t USART, uint8_t data);
