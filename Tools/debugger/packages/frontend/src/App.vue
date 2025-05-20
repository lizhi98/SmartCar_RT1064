<script setup lang="ts">
import type { z } from 'zod'
import axios, { AxiosError } from 'axios'
import {
  parseAnsiSequences, type ParseToken as AnsiParseToken, type Color as AnsiColor,
  type ColorName
} from 'ansi-sequence-parser'

import { computed, nextTick, onMounted, onUnmounted, reactive, ref, useTemplateRef, watch } from 'vue'
import Await from './components/Await.vue'

import { Api, Dataset, Session, CompileRes } from 'car-debugger-shared'

const WIDTH = 188
const HEIGHT = 120
const Y_INSET = 35
const BACKEND_URL = import.meta.env.VITE_BACKEND_URL || `//${location.hostname}:1660`

const http = axios.create({
  baseURL: BACKEND_URL,
})

const api = async <P extends keyof typeof Api>(path: P, body: z.infer<Api[P]['req']>): Promise<z.infer<Api[P]['res']>> => {
  try {
    const { data } = await http({
      method: Api[path].method,
      responseType: 'json',
      url: path,
      data: body,
    })
    return data
  }
  catch (err) {
    if ((err as AxiosError).status === 403) {
      session.value = null
    }
    throw err
  }
}

const session = reactive<{ value: Session | null }>({ value: null })

const datasetList = ref<Dataset[] | null>(null)
const dataset = ref<Dataset | null>(null)

const getDatasetList = async () => {
  return datasetList.value = await api('/dataset-list', {})
}

const createSession = async () => {
  session.value = await api('/create-session', {
    datasetName: dataset.value!.name,
  })
  await runFrame()
}
const taskCreateSession = ref<ReturnType<typeof createSession> | null>(null)

const cellWidth = ref(3)
const cellHeight = ref(3)

const nextFrame = async () => {
  const sess = session.value
  if (! sess) return
  const { size } = dataset.value!
  sess.frameIndex = (sess.frameIndex + 1) % size

  await runFrame()
}

const prevFrame = async () => {
  const sess = session.value
  if (! sess) return
  const { size } = dataset.value!
  sess.frameIndex = (sess.frameIndex - 1 + size) % size

  await runFrame()
}

const compile = async () => {
  const sess = session.value
  if (! sess) return

  const { id } = sess
  const result = compileResult.value = await api('/compile', { id })
  if (result.status === 'success') {
    await runFrame({ doNotClearCompileResult: true })
  }
  else {
    imageOutput.value = null
  }
}

const isAutoPlaying = ref(false)
const interval = ref(50)

const speedUp = () => {
  if (interval.value > 50) interval.value -= 50
}
const speedDown = () => {
  if (interval.value < 2000) interval.value += 50
}

const autoPlay = () => {
  if (isAutoPlaying.value) nextFrame()
  setTimeout(autoPlay, interval.value)
}

onMounted(() => {
  autoPlay()
})

const debugOutput = ref<string | null>(null)
const imageOutput = ref<AnsiParseToken[][] | null>(null)
const frameError = ref<string | null>(null)
const dataStr = ref<string | null>(null)
const isLoadingFrame = ref(false)
const loadedFrameIndex = ref(1)
const loadedFrameBackground = computed(() =>
  session.value
    ? `url(${BACKEND_URL}/dataset-image/${session.value.datasetName}_${loadedFrameIndex.value})`
    : undefined
)
const loadingTime = ref(0)
const compileResult = ref<CompileRes | null>(null)

const canvas = useTemplateRef('canvas')

watch([ imageOutput, cellWidth, cellHeight ], async ([ matrix, cw, ch ]) => {
  if (! canvas.value || ! matrix) return
  await nextTick()

  const ctx = canvas.value.getContext('2d')!
  ctx.resetTransform()
  ctx.scale(cw, ch)
  ctx.clearRect(0, 0, WIDTH, HEIGHT)

  if (displayMode.value === 'image') return

  for (const [ y0, row ] of matrix.entries()) {
    const y = y0 + Y_INSET
    for (const [ x, cell ] of row.entries()) {
      const color = getColor(cell.background, x, y)
      if (color) ctx.fillStyle = color
      ctx.fillRect(x, y, 1, 1)
    }
  }
})

const runFrame = async ({
  doNotClearCompileResult = false,
}: {
  doNotClearCompileResult?: boolean
} = {}) => {
  const sess = session.value
  if (! sess) return

  if (compileResult.value && ! doNotClearCompileResult) {
    compileResult.value = null
  }

  if (isLoadingFrame.value) return

  isLoadingFrame.value = true
  const startTime = Date.now()
  try {

    const { id, frameIndex } = sess
    const result = await api('/run-frame', {
      id,
      frameIndex,
    })
    if (result.status === 'failure') {
      debugOutput.value = imageOutput.value = null
      frameError.value = result.reason
      isLoadingFrame.value = false
      return
    }
    const { stdout, stderr } = result.value
    const [ debugStr, imageStr ] = stdout.split('\x1B[2J\x1B[H')
    debugOutput.value = debugStr
    imageOutput.value = imageStr
      .trim()
      .split('\n')
      .map(line => parseAnsiSequences(line.slice(4)))
    frameError.value = null
    dataStr.value = stderr
    loadedFrameIndex.value = frameIndex
  }
  finally {
    isLoadingFrame.value = false
    loadingTime.value = Date.now() - startTime
  }
}

onUnmounted(async () => {
  if (! session.value) return
  const { id } = session.value
  await api('/delete-session', { id })
})

const COLOR_MAP: Record<ColorName, string> = {
  black: '#000000',
  red: '#FF0000',
  green: '#00FF00',
  yellow: '#FFFF00',
  blue: '#0000FF',
  magenta: '#FF00FF',
  cyan: '#00FFFF',
  white: '#FFFFFF',
  brightBlack: '#808080',
  brightRed: '#FF5555',
  brightGreen: '#50FA7B',
  brightYellow: '#F1FA8C',
  brightBlue: '#BD93F9',
  brightMagenta: '#FF79C6',
  brightCyan: '#8BE9FD',
  brightWhite: '#FFFFFF',
}

const OPACITY = '55'
const MINOR_COLOR_NAMES = [ 'black', 'white' ]

const getColor = (color: AnsiColor | null, x: number, y: number): string | undefined => {
  if (! color) return undefined
  if (color.type === 'rgb') {
    const [ r, g, b ] = color.rgb
    return `rgb(${r}, ${g}, ${b})`
  }
  else if (color.type === 'named') {
    const hex = COLOR_MAP[color.name]
    if (MINOR_COLOR_NAMES.includes(color.name)) {
      if (lineXs[x] || lineYs[y]) return COLOR_MAP.brightMagenta
      return hex + OPACITY
    }
    return hex
  }
}

const DISPLAY_MODES = [ 'both', 'image', 'text' ] as const
const displayMode = ref<typeof DISPLAY_MODES[number]>(DISPLAY_MODES[0])
const nextDisplayMode = () => {
  const index = DISPLAY_MODES.indexOf(displayMode.value)
  displayMode.value = DISPLAY_MODES[(index + 1) % DISPLAY_MODES.length]
}

const lineX = ref<number | null>(null)
const lineY = ref<number | null>(null)
const lineXs = reactive<Record<number, boolean>>({})
const lineYs = reactive<Record<number, boolean>>({})
const toggleLineX = () => {
  if (lineX.value === null) return
  lineXs[lineX.value] = ! lineXs[lineX.value]
}
const toggleLineY = () => {
  if (lineY.value === null) return
  lineYs[lineY.value] = ! lineYs[lineY.value]
}
</script>

<template>
  <div class="session-state">
    <header>
      <Await :promise="getDatasetList()">
        <template #pending>
          Loading dataset index...
        </template>
        <template #fulfilled="{ value: datasetIndex }">
          <div class="flex">
            <span>
              Dataset:
              <select v-model="dataset">
                <option v-for="dataset of datasetIndex" :value="dataset">{{ dataset.name }}</option>
              </select>
            </span>
            <button
              v-if="! session.value"
              :disabled="! dataset"
              @click="taskCreateSession = createSession()"
            >Start debugging</button>
          </div>
        </template>
      </Await>

      <Await v-if="taskCreateSession" :promise="taskCreateSession">
        <template #pending>
          Connecting to {{ BACKEND_URL }}...
        </template>
      </Await>
    </header>

    <main v-if="session.value">
      <div class="flex control-buttons">
        <button :disabled="isLoadingFrame" @click="prevFrame">&lt;</button>
        <input :disabled="isLoadingFrame" type="number" v-model="session.value.frameIndex" />
        <button :disabled="isLoadingFrame" @click="runFrame()">@</button>
        <button :disabled="isLoadingFrame" @click="compile">C</button>
        <button @click="isAutoPlaying = ! isAutoPlaying">{{ isAutoPlaying ? '||' : '|>' }}</button>
        <button :disabled="isLoadingFrame" @click="nextFrame">&gt;</button>
        <button @click="speedUp">S+</button>
        <button @click="speedDown">S-</button>
        <button @click="cellWidth ++">W+</button>
        <button @click="cellWidth --">W-</button>
        <button @click="cellHeight ++">H+</button>
        <button @click="cellHeight --">H-</button>
      </div>
      <div class="flex control-buttons">
        <button @click="nextDisplayMode">{{ displayMode[0] }}</button>
        <input type="number" v-model="lineX" />
        <button @click="toggleLineX">LX</button>
        <input type="number" v-model="lineY" />
        <button @click="toggleLineY">LY</button>
      </div>

      <div class="result flex">
        <div class="box image-output">
          <canvas
            :width="WIDTH * cellWidth"
            :height="HEIGHT * cellHeight"
            :style="{
              backgroundImage: displayMode === 'text'
                ? undefined
                : loadedFrameBackground
            }"
            ref="canvas"
          ></canvas>
        </div>

        <div v-if="frameError !== null" class="box">
          <pre>Error: {{ frameError }}</pre>
        </div>

        <div class="box">
          <pre>{{ dataStr }}</pre>
          <br />
          <pre>[loading time = {{ loadingTime }} ms]</pre>
          <template v-if="compileResult">
            <pre v-if="compileResult.status === 'success'">[compile OK]</pre>
            <template v-else>
              <pre>[compile ERR]</pre>
              <pre>{{ compileResult.reason }}</pre>
            </template>
          </template>
        </div>

        <div v-if="debugOutput" class="box">
          <pre>{{ debugOutput }}</pre>
        </div>
      </div>
    </main>
  </div>
</template>

<style scoped>
main {
  margin: 1rem 0;
}

.box {
  border: 1px solid #000;
  width: fit-content;
  padding: .5rem;
  margin: .5rem 0;
}

.image-output {
  line-height: 0;
}

.image-output > canvas {
  background-size: 100% 100%;
}

.control-buttons {
  margin: .25rem 0;
}

.control-buttons > input {
  width: 3rem;
  height: 2rem;
  box-sizing: border-box;
}

.control-buttons > button {
  flex-shrink: 0;
  white-space: nowrap;
  height: 2rem;
  width: 2rem;
  font-family: monospace;
}

pre {
  margin: 0;
}

.result {
  flex-wrap: wrap;
  align-items: flex-start;
}
</style>
