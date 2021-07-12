
#include "esp_log.h"

#include "fb/airref_builder.h"
#include "fb/airref_reader.h"


#ifdef ns
#error "ns already used"
#endif

#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(AirRef, x) // Specified in the schema.
#define test_assert(x) do { if (!(x)) { assert(0); return -1; }} while(0)

// current conf

void send_ar_conf(ns(AirRefConf_table_t) * ar_conf, uint8_t *buffer)
{
    //build object configuration edits
}

void receive_m_state(const void *buffer)
{
    //ns(Monster_table_t) monster = ns(Monster_as_root(buffer));
}

void receive_ar_state(const void *buffer)
{
}

void receive_ar_conf(const void *buffer)
{
}

#include "air_ref.h"

void send_m_state(flatcc_builder_t *B, machine_state_t *m_state)
{

    //ns(MachineState_table_t) machineState =
    ns(MachineState_start(B));
    //ns(MachineState_push_start(B));
    ns(MachineState_evaporation_pressure_add(B, 3));

    //ns(MachineState_push_end(B));
    ns(MachineState_ref_t) machineState = ns(MachineState_end(B));

    ns(Content_union_ref_t) content = ns(Content_as_MachineState(machineState));

    ns(Message_create_as_root(B, content));
}

int access_message_buffer(const void *buffer)
{
    // Note that we use the `table_t` suffix when reading a table object
    // as opposed to the `ref_t` suffix used during the construction of
    // the buffer.

    ns(Message_table_t) message = ns(Message_as_root(buffer));
    test_assert(message != 0);
    ESP_LOGI("DECODED", "assert ok");
    if (ns(Message_content_type(message)) == ns(Content_MachineState))
    {
        ESP_LOGI("DECODED", "right content type");
        // Note: root object pointers are NOT the same as the `buffer` pointer.

        // Make sure the buffer is accessible.

        ns(MachineState_table_t) m_state = (ns(MachineState_table_t))ns(Message_content(message));

        int32_t evaporation_pressure = ns(MachineState_evaporation_pressure(m_state));
        int32_t condensation_pressure = ns(MachineState_condensation_pressure(m_state));
        int32_t temperature_gas_scarico = ns(MachineState_temperature_gas_scarico(m_state));
        int32_t temperature_environment = ns(MachineState_temperature_environment(m_state));
        int32_t temperature_gas_ritorno = ns(MachineState_temperature_gas_ritorno(m_state));
        int32_t temperature_extra = ns(MachineState_temperature_extra(m_state));

        ESP_LOGI("DECODED", "evaporation_pressure:%d\t",evaporation_pressure);
        ESP_LOGI("DECODED", "condensation_pressure:%d\t",condensation_pressure);
        ESP_LOGI("DECODED", "temperature_gas_scarico:%d\t",temperature_gas_scarico);
        ESP_LOGI("DECODED", "temperature_environment:%d\t",temperature_environment);
        ESP_LOGI("DECODED", "temperature_gas_ritorno:%d\t",temperature_gas_ritorno);
        ESP_LOGI("DECODED", "temperature_extra:%d\n",temperature_extra);
    }
    else if (ns(Message_content_type(message)) == ns(Content_AirRefConf)){}
    else if (ns(Message_content_type(message)) == ns(Content_AirRefState)){}
    // int16_t mana = ns(Monster_mana(monster));
    // // This is just a const char *, but it also supports a fast length operation.
    // flatbuffers_string_t name = ns(Monster_name(monster));
    // size_t name_len = flatbuffers_string_len(name);

    // test_assert(hp == 300);
    // // Since 150 is the default, we are reading a value that wasn't stored.
    // test_assert(mana == 150);
    // test_assert(0 == strcmp(name, "Orc"));
    // test_assert(name_len == strlen("Orc"));

    // int hp_present = ns(Monster_hp_is_present(monster));     // 1
    // int mana_present = ns(Monster_mana_is_present(monster)); // 0
    // test_assert(hp_present);
    // test_assert(!mana_present);

    // ns(Vec3_struct_t) pos = ns(Monster_pos(monster));
    // // Make sure pos has been set.
    // test_assert(pos != 0);
    // float x = ns(Vec3_x(pos));
    // float y = ns(Vec3_y(pos));
    // float z = ns(Vec3_z(pos));

    // // The literal `f` suffix is important because double literals does
    // // not always map cleanly to 32-bit represention even with only a few digits:
    // // `1.0 == 1.0f`, but `3.2 != 3.2f`.
    // test_assert(x == 1.0f);
    // test_assert(y == 2.0f);
    // test_assert(z == 3.0f);

    // // We can also read the position into a C-struct. We have to copy
    // // because we generally do not know if the native endian format
    // // matches the one stored in the buffer (pe: protocol endian).
    // ns(Vec3_t) pos_vec;
    // // `pe` indicates endian conversion from protocol to native.
    // ns(Vec3_copy_from_pe(&pos_vec, pos));
    // test_assert(pos_vec.x == 1.0f);
    // test_assert(pos_vec.y == 2.0f);
    // test_assert(pos_vec.z == 3.0f);

    // // This is a const uint8_t *, but it shouldn't be accessed directly
    // // to ensure proper endian conversion. However, uint8 (ubyte) are
    // // not sensitive endianness, so we *could* have accessed it directly.
    // // The compiler likely optimizes this so that it doesn't matter.
    // flatbuffers_uint8_vec_t inv = ns(Monster_inventory(monster));
    // size_t inv_len = flatbuffers_uint8_vec_len(inv);
    // // Make sure the inventory has been set.
    // test_assert(inv != 0);
    // // If `inv` were absent, the length would 0, so the above test is redundant.
    // test_assert(inv_len == 10);
    // // Index 0 is the first, index 2 is the third.
    // // NOTE: C++ uses the `Get` terminology for vector elemetns, C use `at`.
    // uint8_t third_item = flatbuffers_uint8_vec_at(inv, 2);
    // test_assert(third_item == 2);

    // ns(Weapon_vec_t) weapons = ns(Monster_weapons(monster));
    // size_t weapons_len = ns(Weapon_vec_len(weapons));
    // test_assert(weapons_len == 2);
    // // We can use `const char *` instead of `flatbuffers_string_t`.
    // const char *second_weapon_name = ns(Weapon_name(ns(Weapon_vec_at(weapons, 1))));
    // int16_t second_weapon_damage = ns(Weapon_damage(ns(Weapon_vec_at(weapons, 1))));
    // test_assert(second_weapon_name != 0 && strcmp(second_weapon_name, "Axe") == 0);
    // test_assert(second_weapon_damage == 5);

    // // Access union type field.
    // if (ns(Monster_equipped_type(monster)) == ns(Equipment_Weapon))
    // {
    //     // Cast to appropriate type:
    //     // C does not require the cast to Weapon_table_t, but C++ does.
    //     ns(Weapon_table_t) weapon = (ns(Weapon_table_t))ns(Monster_equipped(monster));
    //     const char *weapon_name = ns(Weapon_name(weapon));
    //     int16_t weapon_damage = ns(Weapon_damage(weapon));

    //     test_assert(0 == strcmp(weapon_name, "Axe"));
    //     test_assert(weapon_damage == 5);
    // }
    return 0;
}