import path from 'node:path'
import fsSync from 'node:fs'
import fs from 'node:fs/promises'

import Fastify from 'fastify'
import cors from '@fastify/cors'
import { type ZodTypeProvider, validatorCompiler, serializerCompiler } from 'fastify-type-provider-zod'

import { pino } from 'pino'
import { execa, ExecaError } from 'execa'

import { CompileReq, CompileRes, CreateSessionReq, CreateSessionRes, DatasetListRes, DeleteSessionReq, DeleteSessionRes, RunFrameReq, RunFrameRes, Session } from 'car-debugger-shared'
import { z } from 'zod'

const logger = pino({
  level: 'debug',
  transport: {
    target: 'pino-pretty',
    options: {
      colorize: true
    }
  }
})

const panic = (msg: string, ...args: any[]) => {
  logger.error(msg, ...args)
  process.exit(1)
}

class SessionNotFoundError extends Error {
  constructor(id: string) {
    super(`Session ${id} not found`)
  }
}

export const fastify = Fastify({
  loggerInstance: logger,
})
  .register(cors)
  .setValidatorCompiler(validatorCompiler)
  .setSerializerCompiler(serializerCompiler)
  .setErrorHandler((err, _req, res) => {
    if (err instanceof SessionNotFoundError) {
      res.status(403)
      return {
        error: err.message,
      }
    }
    return err
  })
  .withTypeProvider<ZodTypeProvider>()

const testDirPath = import.meta.env.VITE_TEST_DIR
if (! testDirPath) {
  panic('$VITE_TEST_DIR is not set')
}
const executablePath = path.join(testDirPath, 'image_test.out')
const datasetPath = path.join(testDirPath, 'Data')
const datasetArrayPath = path.join(datasetPath, 'Array')

const runSession = async (session: Session) => {
  const { frameIndex, prevDataStr, controlStr } = session
  const { stdout, stderr } = await execa(
    executablePath,
    [
      `${datasetArrayPath}/${session.datasetName}_${frameIndex}.dat`,
      prevDataStr,
      controlStr,
    ],
  )
  return { stdout, stderr }
}

const sessions = new Map<string, Session>()

fastify.get('/dataset-list', {
  schema: {
    response: {
      200: DatasetListRes
    }
  }
}, async () => {
  return JSON.parse(
    await fs.readFile(path.join(datasetPath, 'index.json'), 'utf-8')
  )
})

fastify.get('/dataset-image/:file', {
  schema: {
    params: z.object({
      file: z.string()
    })
  }
}, async (req, res) => {
  const { file } = req.params
  const imagePath = path.join(datasetPath, 'Images', `${file}.jpg`)
  const imageStream = fsSync.createReadStream(imagePath)
  return res
    .type('image/jpeg')
    .send(imageStream)
})

fastify.post('/compile', {
  schema: {
    body: CompileReq,
    response: {
      200: CompileRes
    }
  }
}, async () => {
  try {
    await execa(path.join(testDirPath, 'compile.sh'), {
      cwd: testDirPath,
    })
    return {
      status: 'success' as const
    }
  }
  catch (err) {
    const { stderr } = err as ExecaError
    return {
      status: 'failure' as const,
      reason: stderr as string,
    }
  }
})

fastify.post('/create-session', {
  schema: {
    body: CreateSessionReq,
    response: {
      200: CreateSessionRes,
    }
  }
}, async (req) => {
  const { datasetName } = req.body
  const id = crypto.randomUUID()
  const session: Session = {
    id,
    datasetName,
    frameIndex: 1,
    prevDataStr: '',
    controlStr: '',
  }

  sessions.set(id, session)

  return session
})

fastify.post('/delete-session', {
  schema: {
    body: DeleteSessionReq,
    response: {
      200: DeleteSessionRes,
    }
  }
}, async (req) => {
  const { id } = req.body
  const session = sessions.get(id)
  if (! session) throw new SessionNotFoundError(id)

  sessions.delete(id)

  return {
    status: 'success' as const,
  }
})

fastify.post('/run-frame', {
  schema: {
    body: RunFrameReq,
    response: {
      200: RunFrameRes,
    }
  }
}, async (req) => {
  const { id, frameIndex } = req.body
  const session = sessions.get(id)
  if (! session) throw new SessionNotFoundError(id)
  if (frameIndex !== undefined) {
    session.frameIndex = frameIndex
  }

  try {
    await fs.stat(executablePath)
  }
  catch {
    return {
      status: 'failure' as const,
      reason: 'Executable not found'
    }
  }

  try {
    const result = await runSession(session)
    session.prevDataStr = result.stderr
    return {
      status: 'success' as const,
      value: result
    }
  }
  catch (err) {
    const { stderr } = err as ExecaError
    return {
      status: 'failure' as const,
      reason: stderr as string,
    }
  }
})

if (import.meta.env.PROD) {
  fastify.listen({
    port: 1660
  })
}
