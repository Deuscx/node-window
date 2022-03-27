<template>
  <div>
    <ul>
      <li
        @click="handleSelect(id)"
        v-for="{ name, id } of sources"
        :key="id"
        :class="{ active: id === selected }"
      >
        {{ name }} - {{ id }}
      </li>
    </ul>
  </div>
</template>

<script setup>
// import { useIntervalFn } from "@vueuse/core";
import { onMounted, ref, watch } from "vue";

const selected = ref("");
const sources = ref();
/* your function */
onMounted(async () => {
  sources.value = await window.api.desktopCapturer({
    types: ["window"],
  });
});

function handleSelect(id) {
  selected.value = id;
}

watch(selected, async (val, old) => {
  if (val) {
    console.log("🚀 ~ file: HelloWorld.vue ~ line 35 ~ watch ~ val", val);
    if (old !== val && old) {
      await window.api.stop();
    }
    window.api.start(val);
  }
});
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
h3 {
  margin: 40px 0 0;
}
ul {
  list-style-type: none;
  padding: 0;
}
li {
  display: inline-block;
  margin: 0 10px;
}
.active {
  color: #42b983;
}
</style>