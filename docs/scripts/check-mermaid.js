#!/usr/bin/env node

const fs = require('fs');
const path = require('path');

// markdown ファイルを再帰的に探す
function findMarkdownFiles(dir) {
	const files = [];
	const items = fs.readdirSync(dir);

	for (const item of items) {
		const fullPath = path.join(dir, item);
		const stat = fs.statSync(fullPath);

		if (stat.isDirectory() && !item.startsWith('.') && item !== 'node_modules') {
			files.push(...findMarkdownFiles(fullPath));
		} else if (item.endsWith('.md')) {
			files.push(fullPath);
		}
	}

	return files;
}

// Mermaid ブロックを抽出
function extractMermaidBlocks(content, filePath) {
	const regex = /```mermaid\n([\s\S]*?)\n```/g;
	const blocks = [];
	let match;

	while ((match = regex.exec(content)) !== null) {
		blocks.push({
			code: match[1],
			lineNum: content.substring(0, match.index).split('\n').length
		});
	}

	return blocks;
}

// 基本的なパースチェック
function checkMermaidSyntax(code) {
	const errors = [];

	// タイプを判定
	const firstLine = code.trim().split('\n')[0];
	const type = firstLine.trim();

	if (!['flowchart', 'graph', 'sequenceDiagram', 'stateDiagram-v2', 'classDiagram', 'erDiagram', 'gantt'].some(t => type.includes(t))) {
		errors.push('Unknown diagram type');
	}

	// 括弧のバランスをチェック
	let braceCount = 0;
	for (let char of code) {
		if (char === '{') braceCount++;
		if (char === '}') braceCount--;
		if (braceCount < 0) {
			errors.push('Unmatched closing brace');
			break;
		}
	}
	if (braceCount !== 0) {
		errors.push('Unmatched opening brace');
	}

	// end キーワードのバランスをチェック（sequenceDiagram の場合）
	if (type.includes('sequenceDiagram')) {
		const altCount = (code.match(/\balt\b/g) || []).length;
		const endCount = (code.match(/\bend\b/g) || []).length;
		if (altCount > endCount) {
			errors.push('Missing "end" keyword');
		}
	}

	return errors;
}

// メイン処理
function checkMermaidDiagrams() {
	const docsDir = path.dirname(__dirname);
	const mdFiles = findMarkdownFiles(docsDir);

	let hasErrors = false;
	let checkedCount = 0;

	for (const file of mdFiles) {
		const content = fs.readFileSync(file, 'utf-8');
		const blocks = extractMermaidBlocks(content, file);

		if (blocks.length === 0) continue;

		console.log(`\n📄 ${path.relative(docsDir, file)}`);

		for (let i = 0; i < blocks.length; i++) {
			const errors = checkMermaidSyntax(blocks[i].code);

			if (errors.length === 0) {
				console.log(`  ✓ Diagram ${i + 1} (line ${blocks[i].lineNum}): OK`);
				checkedCount++;
			} else {
				console.error(`  ✗ Diagram ${i + 1} (line ${blocks[i].lineNum}):`);
				errors.forEach(err => console.error(`    - ${err}`));
				hasErrors = true;
			}
		}
	}

	console.log(`\n✅ チェック完了: ${checkedCount} 個の Mermaid ダイアグラムをスキャン`);

	if (hasErrors) {
		console.error('\n❌ シンタックスエラーが見つかりました\n');
		process.exit(1);
	}
}

checkMermaidDiagrams();
