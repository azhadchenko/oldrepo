SET_COMMAND_PUSH(AX, {push(&object->stack, object->ax); comm_ptr++;})
SET_COMMAND_PUSH(BX, {push(&object->stack, object->bx); comm_ptr++;})
SET_COMMAND_PUSH(CX, {push(&object->stack, object->cx); comm_ptr++;})
SET_COMMAND_PUSH(DX, {push(&object->stack, object->dx); comm_ptr++;})
SET_COMMAND_PUSH(BP, {push(&object->stack, object->bp); comm_ptr++;})
SET_COMMAND_PUSH(SP, {push(&object->stack, object->sp); comm_ptr++;})


